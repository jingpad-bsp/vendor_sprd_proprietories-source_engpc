#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "crc16.h"
#include "sprd_fts_diag.h"
#include "sprd_fts_log.h"
#include "bt_wifi_mac.h"
#include "translate_packet.h"

int btwifi_checkCRC(char* buff){
    MSG_HEAD_T *msg_head = (MSG_HEAD_T *)(buff + 1);
    int checksum = 0;
    int crc_sum = 0;

    crc_sum =*(unsigned short *)&(buff[msg_head->len - sizeof(unsigned short) + 1]);
    checksum = crc16(0, (unsigned char *)(msg_head + 1), msg_head->len - sizeof(MSG_HEAD_T) - sizeof(unsigned short));
    if (crc_sum != checksum){
        ENG_LOG("%s: CRC Error! crc_sum: %d, checksum: %d\n", __FUNCTION__, crc_sum, checksum);
        return -1;
    }

    return 0;
}

int btwifi_checkValid(char *buff){
    MSG_HEAD_T *msg_head = (MSG_HEAD_T *)(buff + 1);
    int cmd_mask;

    if (0 != btwifi_checkCRC(buff)){
        return 0;
    }

    if ((msg_head->subtype & RW_MASK) == WRITE_MODE && (msg_head->subtype & RM_VALID_CMD_MASK) == 0) {
        return 0;
    }

    if (0 != (cmd_mask = (msg_head->subtype & 0x7F))) {
        ENG_LOG("%s: cmd_mask: %d, subtype: %d\n", __FUNCTION__, cmd_mask, msg_head->subtype);

        if ((cmd_mask & DIAG_CMD_BTBIT) || (cmd_mask & DIAG_CMD_WIFIBIT)) {
            ENG_LOG("%s: Get BT/WIFI Mac addr req or IMEI req!\n", __FUNCTION__);
            if (cmd_mask = (cmd_mask & (~(DIAG_CMD_BTBIT | DIAG_CMD_WIFIBIT)))) {
                ENG_LOG("%s: Have other commands !\n", __FUNCTION__);
                return 2;
            } else {
                ENG_LOG("%s: No other commands !\n", __FUNCTION__);
                return 1;
            }
        }
    }

    return 0;
}

int btwifiimei(char *buf, int len, char *rsp, int rsplen, int cp_ap_proc, int ap_cali) {
    int rlen = 0, i;
    int ret = -1;
    int cmd_mask = 0;
    unsigned short crc = 0;
    unsigned char crc1, crc2, crc3, crc4;
    char tmp;
    char btaddr[32] = {0};
    char wifiaddr[32] = {0};
    char tmprsp[512] = {0};
    char *pBtAddr = NULL, *pWifiAddr = NULL;
    int headlen = sizeof(MSG_HEAD_T);
    REF_NVWriteDirect_T *direct;
    MSG_HEAD_T *head_ptr = NULL;
    head_ptr = (MSG_HEAD_T *)(buf + 1);
    direct = (REF_NVWriteDirect_T *)(buf + DIAG_HEADER_LENGTH + 1);
    cmd_mask = head_ptr->subtype & 0x7f;
    ENG_LOG("Call %s, subtype=%x\n", __FUNCTION__, head_ptr->subtype);

    if ((head_ptr->subtype & DIAG_CMD_READ) == 0) {  // write command
        crc1 = *(buf + DIAG_HEADER_LENGTH + sizeof(REF_NVWriteDirect_T) + 1);
        crc2 = *(buf + DIAG_HEADER_LENGTH + sizeof(REF_NVWriteDirect_T) + 2);
        crc = crc16(crc, (const unsigned char *)direct, sizeof(REF_NVWriteDirect_T));
        crc3 = crc & 0xff;
        crc4 = (crc >> 8) & 0xff;
        ENG_LOG("%s: crc [%x,%x], [%x,%x]\n", __func__, crc3, crc4, crc1, crc2);

        if ((crc1 == crc3) && (crc2 == crc4)) {
            if ((cmd_mask & DIAG_CMD_BTBIT) || (cmd_mask & DIAG_CMD_WIFIBIT)) {
                // write bt address
                if ((head_ptr->subtype & DIAG_CMD_BTBIT) > 0) {
                    sprintf(btaddr, "%02x:%02x:%02x:%02x:%02x:%02x", direct->btaddr[5],
                            direct->btaddr[4], direct->btaddr[3], direct->btaddr[2],
                            direct->btaddr[1], direct->btaddr[0]);
                    pBtAddr = btaddr;
                    ENG_LOG("%s: BTADDR:%s\n", __func__, btaddr);
                }

                // write wifi address
                if ((head_ptr->subtype & DIAG_CMD_WIFIBIT) > 0) {
                    sprintf(wifiaddr, "%02x:%02x:%02x:%02x:%02x:%02x",
                            direct->wifiaddr[0], direct->wifiaddr[1], direct->wifiaddr[2],
                            direct->wifiaddr[3], direct->wifiaddr[4],
                            direct->wifiaddr[5]);
                    pWifiAddr = wifiaddr;
                    ENG_LOG("%s: WIFIADDR:%s\n", __func__, wifiaddr);
                    if(direct->wifiaddr[0] & 0x03){
                        //wifi addr error break;
                        head_ptr->subtype = 0x00;
                        memcpy(tmprsp, (unsigned char *)head_ptr, headlen);
                        head_ptr = (MSG_HEAD_T *)tmprsp;
                        head_ptr->len = headlen + 2;
                        rlen = translate_packet(rsp, tmprsp, head_ptr->len);
                        return rlen;
                    }
                }

                ret = eng_btwifimac_write(pBtAddr, pWifiAddr);
            }

            ENG_LOG("%s: WIFIADDR g_ap_cali_flag:%d,s_cp_ap_proc=%d\n", __func__, ap_cali,cp_ap_proc);
            if (ap_cali &&
                ((cmd_mask & DIAG_CMD_IMEI1BIT) || (cmd_mask & DIAG_CMD_IMEI2BIT) ||
                (cmd_mask & DIAG_CMD_IMEI3BIT) || (cmd_mask & DIAG_CMD_IMEI4BIT))) {
                int imei[IMEI_NUM] = {DIAG_CMD_IMEI1BIT, DIAG_CMD_IMEI2BIT,
                              DIAG_CMD_IMEI3BIT, DIAG_CMD_IMEI4BIT};
                for (i = 0; i < IMEI_NUM; i++) {
                    if (imei[i] & cmd_mask) {
                        ret = eng_diag_write_imei(direct, i + 1);
                        if (ret <= 0) break;
                    }
                }
            }
        }

        if (!cp_ap_proc) {
            if (ret <= 0) {  // fail
                head_ptr->subtype = 0x00;
            } else {
                head_ptr->subtype = 0x01;
            }

            memcpy(tmprsp, (unsigned char *)head_ptr, headlen);
            head_ptr = (MSG_HEAD_T *)tmprsp;
            head_ptr->len = headlen + 2;
            //rlen = translate_packet_encode(rsp, tmprsp, head_ptr->len);
            rlen = translate_packet(rsp, tmprsp, head_ptr->len);
        }
    } else {  // read command
        direct = (REF_NVWriteDirect_T *)(tmprsp + headlen);

        if ((cmd_mask & DIAG_CMD_BTBIT) || (cmd_mask & DIAG_CMD_WIFIBIT)) {
            // read btaddr
            if ((head_ptr->subtype & DIAG_CMD_BTBIT) > 0) {
                ret = eng_btwifimac_read(btaddr, ENG_BT_MAC);
                ENG_LOG("%s: after BTADDR:%s\n", __func__, btaddr);
                pBtAddr = (char *)(direct->btaddr);
                if (!ret) {
                    eng_diag_char2hex((unsigned char *)pBtAddr, btaddr);
                    tmp = pBtAddr[0];
                    pBtAddr[0] = pBtAddr[5];
                    pBtAddr[5] = tmp;  // converge BT address
                    tmp = pBtAddr[1];
                    pBtAddr[1] = pBtAddr[4];
                    pBtAddr[4] = tmp;
                    tmp = pBtAddr[2];
                    pBtAddr[2] = pBtAddr[3];
                    pBtAddr[3] = tmp;
                }
            }
            // read wifiaddr
            if ((head_ptr->subtype & DIAG_CMD_WIFIBIT) > 0) {
                ret = eng_btwifimac_read(wifiaddr, ENG_WIFI_MAC);
                ENG_LOG("%s: after WIFIADDR:%s\n", __func__, wifiaddr);
                pWifiAddr = (char *)(direct->wifiaddr);
                if (!ret) eng_diag_char2hex((unsigned char *)pWifiAddr, wifiaddr);
            }
        }

        if (ap_cali &&
          ((cmd_mask & DIAG_CMD_IMEI1BIT) || (cmd_mask & DIAG_CMD_IMEI2BIT) ||
          (cmd_mask & DIAG_CMD_IMEI3BIT) || (cmd_mask & DIAG_CMD_IMEI4BIT))) {
            int imei[IMEI_NUM] = {DIAG_CMD_IMEI1BIT, DIAG_CMD_IMEI2BIT,
                            DIAG_CMD_IMEI3BIT, DIAG_CMD_IMEI4BIT};
            for (i = 0; i < IMEI_NUM; i++) {
                if (imei[i] & cmd_mask) {
                    ret = eng_diag_read_imei(direct, i + 1);
                    if (ret <= 0) break;
                }
            }
        }
        // response
        head_ptr->subtype = 0x01;
        memcpy(tmprsp, (unsigned char *)head_ptr, headlen);
        head_ptr = (MSG_HEAD_T *)tmprsp;

        rlen = sizeof(REF_NVWriteDirect_T);
        crc = crc16(crc, (const unsigned char *)direct, sizeof(REF_NVWriteDirect_T));

        *(tmprsp + headlen + rlen) = crc & 0xff;
        *(tmprsp + headlen + rlen + 1) = (crc >> 8) & 0xff;
        ENG_LOG("%s: read crc = %d, [%x,%x]\n", __func__, crc,
                *(tmprsp + headlen + rlen), *(tmprsp + headlen + rlen + 1));
        rlen += 2;
        head_ptr->len = headlen + rlen;
        //rlen = translate_packet_encode(rsp, tmprsp, headlen + rlen);
        rlen = translate_packet(rsp, tmprsp, head_ptr->len);
    }

    // clear BT/WIFI bit in this diag framer
    if (cp_ap_proc) {
        head_ptr->subtype &= ~(DIAG_CMD_BTBIT | DIAG_CMD_WIFIBIT);
    }

    ENG_LOG("%s: rlen=%d\n", __func__, rlen);
    return rlen;
}


#define MAC_ERROR_EX ":::::"
#define MAC_ERROR "FF:FF:FF:FF:FF:FF"
#define WIFI_MAC_FILE "/mnt/vendor/wifimac.txt"
#define WIFI_ROOT_MAC_FILE "/data/misc/wifi/wifimac.txt"
#define BT_MAC_FILE "/mnt/vendor/btmac.txt"
#define BT_ROOT_MAC_FILE "/data/misc/bluedroid/btmac.txt"
#define MAC_RAND_FILE "/mnt/vendor/rand_mac.txt"

typedef enum {
    BT_MAC_ADDR=0,
    WIFI_MAC_ADDR
}MAC_ADDR;

static void write_to_randmacfile(char *btmac, char *wifimac) {
  int fd;
  char buf[80];

  fd = open(MAC_RAND_FILE, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd >= 0) {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s;%s", btmac, wifimac);
    write(fd, buf, sizeof(buf));
    close(fd);
  } else {
    ENG_LOG("%s: errno=%d, errstr=%s", __FUNCTION__, errno, strerror(errno));
  }
  ENG_LOG("%s: %s fd=%d, data=%s", __FUNCTION__, MAC_RAND_FILE, fd, buf);
}

// realtek_add_start
static int get_urandom(unsigned int *buf, size_t len) {
  int fd;
  size_t rc;

  ENG_LOG("+%s+", __FUNCTION__);
  fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) {
    ENG_LOG("%s: Open urandom fail", __FUNCTION__);
    return -1;
  }
  rc = read(fd, buf, len);
  close(fd);
  ENG_LOG("-%s: rc: %d-", __FUNCTION__, rc);
  return rc;
}
// realtek_add_end

static void mac_rand(char *btmac, char *wifimac) {
  int fd = 0, i, j, k;
  off_t pos;
  char buf[80];
  char *ptr;
  int ret = 0;
  unsigned int randseed;
  // realtek_add_start
  int rc;
  struct timeval tt;
  // realtek_add_end

  memset(buf, 0, sizeof(buf));

  // realtek_add_start
  ENG_LOG("+%s+", __FUNCTION__);
  // realtek_add_end
  if (access(MAC_RAND_FILE, F_OK) == 0) {
    ENG_LOG("%s: %s exists", __FUNCTION__, MAC_RAND_FILE);
    fd = open(MAC_RAND_FILE, O_RDWR);
    if (fd >= 0) {
      ret = read(fd, buf, sizeof(buf) - 1);
      if (ret > 0) {
        ENG_LOG("%s: read %s %s", __FUNCTION__, MAC_RAND_FILE, buf);
        ptr = strchr(buf, ';');
        if (ptr != NULL) {
          if ((strstr(wifimac, MAC_ERROR) != NULL) ||
              (strstr(wifimac, MAC_ERROR_EX) != NULL) || (strlen(wifimac) == 0))
            strcpy(wifimac, ptr + 1);

          *ptr = '\0';

          if ((strstr(btmac, MAC_ERROR) != NULL) ||
              (strstr(btmac, MAC_ERROR_EX) != NULL) || (strlen(btmac) == 0))
            strcpy(btmac, buf);

          ENG_LOG("%s: read btmac=%s, wifimac=%s", __FUNCTION__, btmac, wifimac);
          close(fd);
          return;
        }
      } else {
        ENG_LOG("%s: read failed", __FUNCTION__);
      }
      // realtek_add_start
      close(fd);
      // realtek_add_end
    }
  }

  rc = get_urandom(&randseed, sizeof(randseed));
  if (rc > 0) {
    ENG_LOG("urandom:%u", randseed);
  } else {
    if (gettimeofday(&tt, (struct timezone *)0) > 0)
      randseed = (unsigned int)tt.tv_usec;
    else
      randseed = (unsigned int)time(NULL);

    ENG_LOG("urandom fail, using system time for randseed");
  }
  // realtek_add_end
  ENG_LOG("%s: randseed=%u", __FUNCTION__, randseed);
  srand(randseed);
  ENG_LOG("%s: mac=%s, fd[%s]=%d", __FUNCTION__, btmac, BT_MAC_FILE, fd);

  // FOR BT
  i = rand();
  j = rand();
  ENG_LOG("%s:  rand i=0x%x, j=0x%x", __FUNCTION__, i, j);
  sprintf(btmac, "00:%02x:%02x:%02x:%02x:%02x",
          (unsigned char)((i >> 8) & 0xFF), (unsigned char)((i >> 16) & 0xFF),
          (unsigned char)((j)&0xFF), (unsigned char)((j >> 8) & 0xFF),
          (unsigned char)((j >> 16) & 0xFF));

  // FOR WIFI
  i = rand();
  j = rand();
  ENG_LOG("%s:  rand i=0x%x, j=0x%x", __FUNCTION__, i, j);
  sprintf(wifimac, "00:%02x:%02x:%02x:%02x:%02x",
          (unsigned char)((i >> 8) & 0xFF), (unsigned char)((i >> 16) & 0xFF),
          (unsigned char)((j)&0xFF), (unsigned char)((j >> 8) & 0xFF),
          (unsigned char)((j >> 16) & 0xFF));

  ENG_LOG("%s: bt mac=%s, wifi mac=%s", __FUNCTION__, btmac, wifimac);

  // create rand file
  // write_to_randmacfile(btmac, wifimac);
}

static int write_mac2file(char *wifimac, char *btmac) {
  int fd;
  int ret = 0;

  // wifi mac
  fd = open(WIFI_MAC_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
  ENG_LOG("%s: mac=%s, fd[%s]=%d,errno=%d, errstr=%s", __FUNCTION__, wifimac,
          WIFI_MAC_FILE, fd, errno, strerror(errno));
  if (fd >= 0) {
    if (-1 == chmod(WIFI_MAC_FILE, 0666))
      ENG_LOG("%s chmod failed", __FUNCTION__);
    ret = write(fd, wifimac, strlen(wifimac));
    ENG_LOG("%s: write wifimac=%s, ret=%d", __FUNCTION__, wifimac, ret);
    close(fd);
  } else {
    ret = -1;
    return ret;
  }

  // bt mac
  fd = open(BT_MAC_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
  ENG_LOG("%s: mac=%s, fd[%s]=%d,errno=%d, errstr=%s", __FUNCTION__, btmac,
          BT_MAC_FILE, fd, errno, strerror(errno));
  if (fd >= 0) {
    if (-1 == chmod(BT_MAC_FILE, 0666))
      ENG_LOG("%s chmod failed", __FUNCTION__);
    ret = write(fd, btmac, strlen(btmac));
    ENG_LOG("%s: write btmac=%s, ret=%d", __FUNCTION__, btmac, ret);
    close(fd);
    if (0 == access(BT_ROOT_MAC_FILE, F_OK)) {
      if (-1 == remove(BT_ROOT_MAC_FILE)) {
        ENG_LOG("%s remove  %s failed", __FUNCTION__, BT_ROOT_MAC_FILE);
      }
    }
  } else {
    ret = -1;
  }

  sync();

  return ret;
}

int eng_btwifimac_write(char *bt_mac, char *wifi_mac) {
  int ret = 0;
  char bt_mac_rand[32] = {0};
  char wifi_mac_rand[32] = {0};

  ENG_LOG("set BT/WIFI mac");

  // If no bt_mac or wifi_mac, we can randomly generate them.
  if (!bt_mac || !wifi_mac) {
    mac_rand(bt_mac_rand, wifi_mac_rand);
    if (!bt_mac) {
      eng_btwifimac_read(bt_mac_rand, ENG_BT_MAC);
      bt_mac = bt_mac_rand;
    }
    if (!wifi_mac) {
      eng_btwifimac_read(wifi_mac_rand, ENG_WIFI_MAC);
      wifi_mac = wifi_mac_rand;
    }
  }

  ENG_LOG("property ro.mac.wifi=%s, ro.mac.bluetooth=%s", wifi_mac, bt_mac);
  ret = write_mac2file(wifi_mac, bt_mac);

  if (ret > 0) {
    property_set("vendor.sys.mac.wifi", wifi_mac);
    property_set("vendor.sys.mac.bluetooth", bt_mac);
    property_set("vendor.sys.bt.bdaddr_path", BT_MAC_FILE);
    property_set("ctl.start", "set_mac");
  }

  return ret;
}

int eng_btwifimac_read(char *mac, MacType type) {
  int fd, rcount;
  int ret = 0;

  if (!mac) return -1;

  if (ENG_WIFI_MAC == type) {
    // read wifi mac
    if (access(WIFI_MAC_FILE, F_OK) == 0) {
      ENG_LOG("%s: %s exists", __FUNCTION__, WIFI_MAC_FILE);
      fd = open(WIFI_MAC_FILE, O_RDONLY);
    } else {
      ENG_LOG("%s: %s not exists,read %s", __FUNCTION__, WIFI_MAC_FILE,
              WIFI_ROOT_MAC_FILE);
      fd = open(WIFI_ROOT_MAC_FILE, O_RDONLY);
    }
  } else {
    // read bt mac
    if (access(BT_MAC_FILE, F_OK) == 0) {
      ENG_LOG("%s: %s exists", __FUNCTION__, BT_MAC_FILE);
      fd = open(BT_MAC_FILE, O_RDONLY);
    } else {
      ENG_LOG("%s: %s not exists,read %s", __FUNCTION__, BT_MAC_FILE,
              BT_ROOT_MAC_FILE);
      fd = open(BT_ROOT_MAC_FILE, O_RDONLY);
    }
  }

  if (fd >= 0) {
    rcount = read(fd, mac, 31);
    if (rcount <= 0) {
      ret = -1;
    }

    ENG_LOG("%s: mac=%s, fd[%s]=%d", __FUNCTION__, mac, WIFI_MAC_FILE, fd);
    close(fd);
  } else {
    ENG_LOG("%s: fd=%d,errno=%d, strerror(errno)=%s", __FUNCTION__, fd, errno,
            strerror(errno));
    ret = -1;
  }

  return ret;
}

void eng_diag_char2hex(unsigned char *hexdata, char *chardata) {
  int i, index = 0;
  char *ptr;
  char tmp[4];

  while ((ptr = strchr(chardata, ':')) != NULL) {
    snprintf(tmp, 3, "%s", chardata);
    hexdata[index++] = strtol(tmp, NULL, 16);
    chardata = ptr + 1;
  }

  hexdata[index++] = strtol(chardata, NULL, 16);

  for (i = 0; i < index; i++) {
    ENG_LOG("%s: [%d]=0x%x\n", __FUNCTION__, i, hexdata[i]);
  }
}

int eng_diag_write_imei(REF_NVWriteDirect_T *direct, int num) {
  int ret = 0;
  int fd = -1;
  char imei_path[ENG_DEV_PATH_LEN] = {0};
  char imeinv[MAX_IMEI_LENGTH] = {0};
  char imeistr[MAX_IMEI_STR_LENGTH] = {0};

  ENG_LOG("%s: imei num: %d\n", __FUNCTION__, num);

  switch (num) {
    case 1:
      strcpy(imei_path, ENG_IMEI1_CONFIG_FILE);
      memcpy(imeinv, direct->imei1, MAX_IMEI_LENGTH);
      break;
    case 2:
      strcpy(imei_path, ENG_IMEI2_CONFIG_FILE);
      memcpy(imeinv, direct->imei2, MAX_IMEI_LENGTH);
      break;
    case 3:
      strcpy(imei_path, ENG_IMEI3_CONFIG_FILE);
      memcpy(imeinv, direct->imei3, MAX_IMEI_LENGTH);
      break;
    case 4:
      strcpy(imei_path, ENG_IMEI4_CONFIG_FILE);
      memcpy(imeinv, direct->imei4, MAX_IMEI_LENGTH);
      break;
    default:
      return 0;
  }

  fd = open(imei_path, O_WRONLY);
  ENG_LOG("imei_path=%s: fd: %d\n", imei_path, fd);
  if (fd >= 0) {
    ImeiConvNV2Str(imeinv, imeistr);
    ret = write(fd, imeistr, MAX_IMEI_STR_LENGTH);
    if (ret > 0) {
      ret = 1;
      fsync(fd);
    } else {
      ret = 0;
    }
    close(fd);
  }

  return ret;
}

int eng_diag_read_imei(REF_NVWriteDirect_T *direct, int num) {
  int ret = 0;
  int fd = -1;
  char imei_path[ENG_DEV_PATH_LEN] = {0};
  char imeistr[MAX_IMEI_STR_LENGTH] = {0};
  char *imeinv = 0;

  ENG_LOG("%s: imei num: %d\n", __FUNCTION__, num);

  switch (num) {
    case 1:
      strcpy(imei_path, ENG_IMEI1_CONFIG_FILE);
      imeinv = direct->imei1;
      break;
    case 2:
      strcpy(imei_path, ENG_IMEI2_CONFIG_FILE);
      imeinv = direct->imei2;
      break;
    case 3:
      strcpy(imei_path, ENG_IMEI3_CONFIG_FILE);
      imeinv = direct->imei3;
      break;
    case 4:
      strcpy(imei_path, ENG_IMEI4_CONFIG_FILE);
      imeinv = direct->imei4;
      break;
    default:
      return 0;
  }

  fd = open(imei_path, O_RDONLY);
  if (fd >= 0) {
    ret = read(fd, imeistr, MAX_IMEI_STR_LENGTH);
    if (ret > 0) {
      ImeiConvStr2NV(imeistr, imeinv);
      ret = 1;
    } else {
      ret = 0;
    }
    close(fd);
  }

  return ret;
}

void ImeiConvStr2NV(unsigned char *szImei, unsigned char *nvImei) {
  unsigned char temp1 = (unsigned char)(szImei[0] - '0');
  unsigned char temp2 = 0;
  nvImei[0] = (unsigned char)MAKE1BYTE2BYTES(temp1, (unsigned char)0x0A);
  int i;
  for (i = 1; i < MAX_IMEI_LENGTH; i++) {
    temp1 = (unsigned char)(szImei[2 * i - 0] - '0');
    temp2 = (unsigned char)(szImei[2 * i - 1] - '0');
    nvImei[i] = (unsigned char)MAKE1BYTE2BYTES(temp1, temp2);
  }
}

void ImeiConvNV2Str(unsigned char *nvImei, unsigned char *szImei) {
  int i;

  for (i = 0; i < MAX_IMEI_LENGTH - 1; i++) {
    szImei[2 * i + 0] = ((nvImei[i] & 0xF0) >> 4) + '0';
    szImei[2 * i + 1] = (nvImei[i + 1] & 0x0F) + '0';
  }

  szImei[14] = ((nvImei[7] & 0xF0) >> 4) + '0';
}

char MAKE1BYTE2BYTES(unsigned char high4bit, unsigned char low4bit) {
  char temp;
  temp = (high4bit << 4) | low4bit;
  return temp;
}