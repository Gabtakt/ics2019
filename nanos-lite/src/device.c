#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  /* pa3.2 
   * 2020-12-13
   * NOTE: ignore the parameter 'offset'(it is unnecessary for serial)
   */
  int i = 0;
  for ( ; i < len; i++) {
    _putc(((char *)buf)[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  /* pa3.3
   * 2020-12-13
   * NOTE: priority process the key events
   * use api: nexus-am/libs/klib/src/io.c read_key()
   * magic number: 0x8000--keydown if high-bit is 1
   *               0x7fff--set high-bit to 0
   */
  int key = read_key();
  int ret = 0;
  // there is key event
  if (key != _KEY_NONE) {
    // keydown
    if (key & 0x8000) {
      key = key & 0x7fff;
      ret = sprintf(buf, "kd %s\n", keyname[key]);
    }
    else {
      ret = sprintf(buf, "ku %s\n", keyname[key]);
    }
  }
  else {
    ret = sprintf(buf, "t %u\n", uptime());
  }
  return ret;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
