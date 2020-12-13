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
  /* pa3.3
   * 2020-12-13
   */
  if (len > sizeof(dispinfo) - offset) {
    len = sizeof(dispinfo) - offset;
  }
  strncpy((char *)buf, &dispinfo, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  /* pa3.3
   * 2020-12-13
   */
  // int dx, dy;
  // int width = screen_width();
  // size_t pixels_num = len >> 2;
  // offset = offset >> 2;
  // dy = offset / width;
  // dx = offset % width;
  // draw_rect((uint32_t *)buf, dx, dy, pixels_num, 1);
  // return 0;
  //Log("fb_write\n");
  
  int x,y;
  offset = offset>>2;
  y = offset / screen_width();
  x = offset % screen_width();
  //printf("%d %d\n",x,y);
  int lenth = len>>2;
  int len1,len2=0,len3=0;

  len1 = lenth<= (screen_width()-x) ? lenth : screen_width()-x;
  draw_rect((uint32_t*)buf,x,y,len1,1);

  if((lenth>len1)&&((lenth-len1)>screen_width())){
    len2 = lenth-len1;
    draw_rect((uint32_t*)buf+len1,0,y+1,screen_width(),len2/screen_width());
  }

  if(lenth-len1-len2>0){
    len3 = lenth-len1-len2;
    draw_rect((uint32_t*)buf+len1+len2,0,y+len2/screen_width()+1,len3,1);
  }
	//draw_rect((uint32_t *)buf,x,y,lenth,1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  /* pa3.3
   * 2020-12-13
   * print the string to array `dispinfo` with the format
   * described in the Navy-apps convention
   */
  int width = screen_width();
  int height = screen_height();
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", width, height);
}
