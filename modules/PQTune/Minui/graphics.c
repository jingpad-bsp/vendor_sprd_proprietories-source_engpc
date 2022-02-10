/*
 * Copyright (C) 2007 The Android Open Source Project
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

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/Log.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include "minui.h"
#include "graphics.h"

typedef struct {
    GRSurface* texture;
    int cwidth;
    int cheight;
} GRFont;

static minui_backend* gr_backend = NULL;

static unsigned char gr_current_r = 255;
static unsigned char gr_current_g = 255;
static unsigned char gr_current_b = 255;
static unsigned char gr_current_a = 255;
static GRSurface* gr_draw = NULL;

void gr_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    gr_current_r = r;
    gr_current_g = g;
    gr_current_b = b;
    gr_current_a = a;
}

void gr_clear() {
    if (gr_current_r == gr_current_g &&
        gr_current_r == gr_current_b) {
        memset(gr_draw->data, gr_current_r, gr_draw->height * gr_draw->row_bytes);
    } else {
        int x, y;
        unsigned char* px = gr_draw->data;
        for (y = 0; y < gr_draw->height; ++y) {
            for (x = 0; x < gr_draw->width; ++x) {
                *px++ = gr_current_r;
                *px++ = gr_current_g;
                *px++ = gr_current_b;
                px++;
            }
            px += gr_draw->row_bytes - (gr_draw->width * gr_draw->pixel_bytes);
        }
    }
}

void gr_sync(void) {
    if (gr_backend == NULL)
        return;

    if (gr_backend->sync)
        gr_backend->sync(gr_draw);
}

void gr_flip() {
      gr_draw = gr_backend->flip(gr_backend);
}

int gr_init(void) {
	ALOGD("PQ open drm enter\n");
	gr_backend = open_drm();
	gr_draw = gr_backend->init(gr_backend);
	if (gr_draw == NULL) {
		ALOGD("PQ open drm fail\n");
		return -1;
	}
	ALOGD("PQ open drm exit\n");

    return 0;
}

void gr_exit(void) {
    gr_backend->exit(gr_backend);
}

void gr_fb_blank(bool blank) {
    gr_backend->blank(gr_backend, blank);
}
