#include <am.h>
#include <nemu.h>
#include <stdio.h>

uint32_t boot_time_us;
uint32_t boot_time_s;

void __am_timer_init() {
  boot_time_us = inl(RTC_ADDR);
  boot_time_s = inl(RTC_ADDR + 4);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // printf("%d\n", -1);
  uint32_t this_time_us = inl(RTC_ADDR);
  uint32_t this_time_s = inl(RTC_ADDR + 4);
  uint32_t s = this_time_s - boot_time_s;
  uint32_t us = this_time_us - boot_time_us;
  uint64_t res = s * 1000000 + us;
  // printf("%d %d %d\n", this_time_s, this_time_us, res);
  uptime->us = res;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 1;
  rtc->minute = 1;
  rtc->hour   = 1;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
