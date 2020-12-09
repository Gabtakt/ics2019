#include <am.h>
#include <amdev.h>
#include <nemu.h>

/* system set up time */
static uint32_t set_up_time;

size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    /* pa2.3
     * 2020-12-9
     * NOTE: uptime = (hi << 32) | (lo)
     */
    case _DEVREG_TIMER_UPTIME: {
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      // uint64_t time = inl(RTC_ADDR) - set_up_time;
      // uptime->hi = (uint32_t)(time >> 32);
      // uptime->lo = (uint32_t)(time & 0xffffffff);
      return sizeof(_DEV_TIMER_UPTIME_t);
    }
    case _DEVREG_TIMER_DATE: {
      _DEV_TIMER_DATE_t *rtc = (_DEV_TIMER_DATE_t *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2000;
      return sizeof(_DEV_TIMER_DATE_t);
    }
  }
  return 0;
}

void __am_timer_init() {
  //set_up_time = inl(RTC_ADDR);
}
