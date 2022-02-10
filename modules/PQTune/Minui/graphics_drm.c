/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/cdefs.h>
#include <sys/mman.h>

#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "graphics.h"
#include "minui.h"

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(*(A)))

struct drm_surface_pdata {
    GRSurface base;
    uint32_t fb_id;
    uint32_t handle;
};

typedef struct drm_surface_pdata* gr_surface_drm;

struct drm_pdata {
    minui_backend base;
    struct drm_surface_pdata* GRSurfaceDrms[2];
    int current_buffer;
    drmModeCrtc* main_monitor_crtc;
    drmModeConnector* main_monitor_connector;
    int drm_fd;
};

static void DrmDisableCrtc(int drm_fd, drmModeCrtc* crtc) {
  if (crtc) {
    drmModeSetCrtc(drm_fd, crtc->crtc_id,
                   0,         // fb_id
                   0, 0,      // x,y
                   NULL,   // connectors
                   0,         // connector_count
                   NULL);  // mode
  }
}

static void DrmEnableCrtc(struct drm_pdata* pdata,
                          drmModeCrtc* crtc, gr_surface_drm surface) {
  int32_t ret = drmModeSetCrtc(pdata->drm_fd, crtc->crtc_id, surface->fb_id, 0, 0,  // x,y
                               &pdata->main_monitor_connector->connector_id,
                               1,  // connector_count
                               &pdata->main_monitor_crtc->mode);

  if (ret) {
    printf("drmModeSetCrtc failed ret=%d\n", ret);
  }
}

static void drm_blank(struct minui_backend *backend, bool blank) {
  struct drm_pdata *pdata = (struct drm_pdata *)backend;

  if (blank) {
    DrmDisableCrtc(pdata->drm_fd, pdata->main_monitor_crtc);
  } else {
    DrmEnableCrtc(pdata, pdata->main_monitor_crtc,
                  pdata->GRSurfaceDrms[pdata->current_buffer]);
  }
}

static void DrmDestroySurface(int drm_fd, gr_surface_drm surface) {
  if (!surface) return;

  if (surface->base.data) {
    munmap(surface->base.data,
          surface->base.row_bytes * surface->base.height);
  }

  if (surface->fb_id) {
    int ret = drmModeRmFB(drm_fd, surface->fb_id);
    if (ret) {
      printf("drmModeRmFB failed ret=%d\n", ret);
    }
  }

  if (surface->handle) {
    struct drm_gem_close gem_close = {};
    gem_close.handle = surface->handle;

    int ret = drmIoctl(drm_fd, DRM_IOCTL_GEM_CLOSE, &gem_close);
    if (ret) {
      printf("DRM_IOCTL_GEM_CLOSE failed ret=%d\n", ret);
    }
  }
  free(surface);
  surface = NULL;
}

static int drm_format_to_bpp(uint32_t format) {
  switch (format) {
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_BGRA8888:
    case DRM_FORMAT_RGBX8888:
    case DRM_FORMAT_BGRX8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_XRGB8888:
      return 32;
    case DRM_FORMAT_RGB565:
      return 16;
    default:
      printf("Unknown format %d\n", format);
      return 32;
  }
}

static gr_surface_drm DrmCreateSurface(int drm_fd, int width, int height) {
  gr_surface_drm surface = calloc(1, sizeof(*surface));

  uint32_t format;
#if defined(RECOVERY_ABGR)
  format = DRM_FORMAT_RGBA8888;
#elif defined(RECOVERY_BGRA)
  format = DRM_FORMAT_ARGB8888;
#elif defined(RECOVERY_RGBX)
  format = DRM_FORMAT_XBGR8888;
#else
  format = DRM_FORMAT_RGB565;
#endif

  struct drm_mode_create_dumb create_dumb = {};
  create_dumb.height = height;
  create_dumb.width = width;
  create_dumb.bpp = drm_format_to_bpp(format);
  create_dumb.flags = 0;

  int ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb);
  if (ret) {
    printf("DRM_IOCTL_MODE_CREATE_DUMB failed ret=%d\n", ret);
    DrmDestroySurface(drm_fd, surface);
    return NULL;
  }
  surface->handle = create_dumb.handle;

  uint32_t handles[4], pitches[4], offsets[4];

  handles[0] = surface->handle;
  pitches[0] = create_dumb.pitch;
  offsets[0] = 0;

  ret =
      drmModeAddFB2(drm_fd, width, height, format, handles, pitches, offsets, &(surface->fb_id), 0);
  if (ret) {
    printf("drmModeAddFB2 failed ret=%d\n", ret);
    DrmDestroySurface(drm_fd, surface);
    return NULL;
  }

  struct drm_mode_map_dumb map_dumb = {};
  map_dumb.handle = create_dumb.handle;
  ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb);
  if (ret) {
    printf("DRM_IOCTL_MODE_MAP_DUMB failed ret=%d\n", ret);
    DrmDestroySurface(drm_fd, surface);
    return NULL;
  }

  surface->base.height = height;
  surface->base.width = width;
  surface->base.row_bytes = create_dumb.pitch;
  surface->base.pixel_bytes = create_dumb.bpp / 8;
#if defined(TARGET_ARCH_ARM64)
    surface->base.data = (unsigned char*)(mmap64(NULL, surface->base.height *
                                                   surface->base.row_bytes,
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd,
                                                   map_dumb.offset));
#elif defined(TARGET_ARCH_x86_64)
    surface->base.data = (unsigned char*)(mmap64(NULL, surface->base.height *
                                                   surface->base.row_bytes,
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd,
                                                   map_dumb.offset));
#else
    surface->base.data = (unsigned char*)(mmap(NULL, surface->base.height *
                                                   surface->base.row_bytes,
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd,
                                                   map_dumb.offset));
#endif

  if (surface->base.data == MAP_FAILED) {
    DrmDestroySurface(drm_fd, surface);
    return NULL;
  }

  return surface;
}

static drmModeCrtc* find_crtc_for_connector(int fd, drmModeRes* resources,
                                            drmModeConnector* connector) {
  // Find the encoder. If we already have one, just use it.
  drmModeEncoder* encoder;
  if (connector->encoder_id) {
    encoder = drmModeGetEncoder(fd, connector->encoder_id);
  } else {
    encoder = NULL;
  }

  int32_t crtc;
  if (encoder) {
    if (encoder->crtc_id) {
      crtc = encoder->crtc_id;
      drmModeFreeEncoder(encoder);
      return drmModeGetCrtc(fd, crtc);
    } else {
      drmModeFreeEncoder(encoder);
      encoder = NULL;
    }
  }

  // Didn't find anything, try to find a crtc and encoder combo.
  crtc = -1;
  for (int i = 0; i < connector->count_encoders; i++) {
    encoder = drmModeGetEncoder(fd, connector->encoders[i]);

    if (encoder) {
      for (int j = 0; j < resources->count_crtcs; j++) {
        if (!(encoder->possible_crtcs & (1 << j))) continue;
        crtc = resources->crtcs[j];
        break;
      }
      if (crtc >= 0) {
        drmModeFreeEncoder(encoder);
        return drmModeGetCrtc(fd, crtc);
      } else {
        drmModeFreeEncoder(encoder);
        encoder = NULL;
      }
    }
  }

  return NULL;
}

static drmModeConnector* find_used_connector_by_type(int fd, drmModeRes* resources, unsigned type) {
  for (int i = 0; i < resources->count_connectors; i++) {
    drmModeConnector* connector = drmModeGetConnector(fd, resources->connectors[i]);
    if (connector) {
      if ((connector->connector_type == type) && (connector->connection == DRM_MODE_CONNECTED) &&
          (connector->count_modes > 0)) {
        return connector;
      }
      drmModeFreeConnector(connector);
    }
  }
  return NULL;
}

static drmModeConnector* find_first_connected_connector(int fd, drmModeRes* resources) {
  for (int i = 0; i < resources->count_connectors; i++) {
    drmModeConnector* connector;

    connector = drmModeGetConnector(fd, resources->connectors[i]);
    if (connector) {
      if ((connector->count_modes > 0) && (connector->connection == DRM_MODE_CONNECTED))
        return connector;

      drmModeFreeConnector(connector);
    }
  }
  return NULL;
}

static drmModeConnector* FindMainMonitor(int fd, drmModeRes* resources,
                                                   uint32_t* mode_index) {
  /* Look for LVDS/eDP/DSI connectors. Those are the main screens. */
  static unsigned kConnectorPriority[] = {
    DRM_MODE_CONNECTOR_LVDS,
    DRM_MODE_CONNECTOR_eDP,
    DRM_MODE_CONNECTOR_DSI,
  };

  drmModeConnector* main_monitor_connector = NULL;
  unsigned i = 0;
  do {
    main_monitor_connector = find_used_connector_by_type(fd, resources, kConnectorPriority[i]);
    i++;
  } while (!main_monitor_connector && i < ARRAY_SIZE(kConnectorPriority));

  /* If we didn't find a connector, grab the first one that is connected. */
  if (!main_monitor_connector) {
    main_monitor_connector = find_first_connected_connector(fd, resources);
  }

  /* If we still didn't find a connector, give up and return. */
  if (!main_monitor_connector) return NULL;

  *mode_index = 0;
  for (int modes = 0; modes < main_monitor_connector->count_modes; modes++) {
    if (main_monitor_connector->modes[modes].type & DRM_MODE_TYPE_PREFERRED) {
      *mode_index = modes;
      break;
    }
  }

  return main_monitor_connector;
}

static void DisableNonMainCrtcs(int fd, drmModeRes* resources, drmModeCrtc* main_crtc) {
  for (int i = 0; i < resources->count_connectors; i++) {
    drmModeConnector* connector = drmModeGetConnector(fd, resources->connectors[i]);
    drmModeCrtc* crtc = find_crtc_for_connector(fd, resources, connector);

    if (!crtc) return;

    if (crtc->crtc_id != main_crtc->crtc_id) {
      DrmDisableCrtc(fd, crtc);
    }
    drmModeFreeCrtc(crtc);
  }
}

static gr_surface drm_init(struct minui_backend *backend) {
  struct drm_pdata *pdata = (struct drm_pdata *)backend;
  drmModeRes* res = NULL;

  /* Consider DRM devices in order. */
  for (int i = 0; i < DRM_MAX_MINOR; i++) {
    char* dev_name;
    int ret = asprintf(&dev_name, DRM_DEV_NAME, DRM_DIR_NAME, i);
    if (ret < 0) {
      free(dev_name);
      continue;
    }

    pdata->drm_fd = open(dev_name, O_RDWR, 0);
    free(dev_name);
    if (pdata->drm_fd < 0) continue;

    uint64_t cap = 0;
    /* We need dumb buffers. */
    ret = drmGetCap(pdata->drm_fd, DRM_CAP_DUMB_BUFFER, &cap);
    if (ret || cap == 0) {
      close(pdata->drm_fd);
      continue;
    }

    res = drmModeGetResources(pdata->drm_fd);
    if (!res) {
      close(pdata->drm_fd);
      continue;
    }

    /* Use this device if it has at least one connected monitor. */
    if (res->count_crtcs > 0 && res->count_connectors > 0) {
      if (find_first_connected_connector(pdata->drm_fd, res)) break;
    }

    drmModeFreeResources(res);
    close(pdata->drm_fd);
    res = NULL;
  }

  if (pdata->drm_fd < 0 || res == NULL) {
    perror("cannot find/open a drm device");
    return NULL;
  }

  uint32_t selected_mode;
  pdata->main_monitor_connector = FindMainMonitor(pdata->drm_fd, res, &selected_mode);

  if (!pdata->main_monitor_connector) {
    printf("main_monitor_connector not found\n");
    drmModeFreeResources(res);
    close(pdata->drm_fd);
    return NULL;
  }

  pdata->main_monitor_crtc = find_crtc_for_connector(pdata->drm_fd, res,
                                              pdata->main_monitor_connector);

  if (!pdata->main_monitor_crtc) {
    printf("main_monitor_crtc not found\n");
    drmModeFreeResources(res);
    close(pdata->drm_fd);
    return NULL;
  }

  DisableNonMainCrtcs(pdata->drm_fd, res, pdata->main_monitor_crtc);

  pdata->main_monitor_crtc->mode =
                pdata->main_monitor_connector->modes[selected_mode];

  int width = pdata->main_monitor_crtc->mode.hdisplay;
  int height = pdata->main_monitor_crtc->mode.vdisplay;

  drmModeFreeResources(res);

  pdata->GRSurfaceDrms[0] = DrmCreateSurface(pdata->drm_fd, width, height);
  pdata->GRSurfaceDrms[1] = DrmCreateSurface(pdata->drm_fd, width, height);
  if (!pdata->GRSurfaceDrms[0] || !pdata->GRSurfaceDrms[1]) {
    // GRSurfaceDrms and drm_fd should be freed in d'tor.
    return NULL;
  }

  pdata->current_buffer = 0;

  DrmEnableCrtc(pdata, pdata->main_monitor_crtc,
                pdata->GRSurfaceDrms[1]);

  return pdata->GRSurfaceDrms[0];
}

static void page_flip_complete(__unused int fd,
                               __unused unsigned int sequence,
                               __unused unsigned int tv_sec,
                               __unused unsigned int tv_usec,
                               void *user_data) {
  *(bool *)user_data = false;
}

static gr_surface drm_flip(struct minui_backend *backend) {
  struct drm_pdata *pdata = (struct drm_pdata *)backend;
  bool ongoing_flip = true;

  int ret = drmModePageFlip(pdata->drm_fd, pdata->main_monitor_crtc->crtc_id,
                            pdata->GRSurfaceDrms[pdata->current_buffer]->fb_id,
                            DRM_MODE_PAGE_FLIP_EVENT, &ongoing_flip);
  if (ret < 0) {
    printf("drmModePageFlip failed ret=%d\n", ret);
    return NULL;
  }

  while (ongoing_flip) {
    struct pollfd fds = {
      .fd = pdata->drm_fd,
      .events = POLLIN
    };

    ret = poll(&fds, 1, -1);
    if (ret == -1 || !(fds.revents & POLLIN)) {
      printf("poll() failed on drm fd\n");
      break;
    }

    drmEventContext evctx = {
      .version = DRM_EVENT_CONTEXT_VERSION,
      .page_flip_handler = page_flip_complete
    };

    ret = drmHandleEvent(pdata->drm_fd, &evctx);
    if (ret != 0) {
      printf("drmHandleEvent failed ret=%d\n", ret);
      break;
    }
  }

  pdata->current_buffer = 1 - pdata->current_buffer;
  return pdata->GRSurfaceDrms[pdata->current_buffer];
}

static void drm_exit(struct minui_backend *backend) {
    struct drm_pdata *pdata = (struct drm_pdata *)backend;
    unsigned int i;

    for (i = 0; i < 2; i++)
        DrmDestroySurface(pdata->drm_fd, pdata->GRSurfaceDrms[i]);
    if (pdata->drm_fd >= 0)
        close(pdata->drm_fd);
    free(pdata);
}

minui_backend *open_drm() {
    struct drm_pdata *pdata = calloc(1, sizeof(*pdata));
    if (!pdata) {
        perror("allocating drm backend failed");
        return NULL;
    }

    pdata->main_monitor_crtc = NULL;
    pdata->main_monitor_connector = NULL;
    pdata->drm_fd = -1;

    pdata->base.init = drm_init;
    pdata->base.flip = drm_flip;
    pdata->base.blank = drm_blank;
    pdata->base.exit = drm_exit;
    return &pdata->base;
}
