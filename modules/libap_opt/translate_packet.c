#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

int translate_packet_encode(char *dest, char *src, int size) {
  int i;
  int translated_size = 0;

  dest[translated_size++] = 0x7E;

  for (i = 0; i < size; i++) {
    if (src[i] == 0x7E) {
      dest[translated_size++] = 0x7D;
      dest[translated_size++] = 0x5E;
    } else if (src[i] == 0x7D) {
      dest[translated_size++] = 0x7D;
      dest[translated_size++] = 0x5D;
    } else
      dest[translated_size++] = src[i];
  }
  dest[translated_size++] = 0x7E;
  return translated_size;
}

int translate_packet(char *dest, char *src, int size) {

  dest[0] = 0x7E;
  dest++;
  memcpy(dest, src, size);
  dest[size] = 0x7E;

  return size+2;
}
