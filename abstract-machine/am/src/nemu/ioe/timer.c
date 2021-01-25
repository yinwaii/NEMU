#include <am.h>
#include <nemu.h>
#include <stdio.h>

uint32_t boot_time;

void __am_timer_init() {
  boot_time = inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // printf("%d\n", -1);
  uint32_t this_time = inl(RTC_ADDR);
  // uint32_t us = this_time - boot_time;
  // printf("%d %d %d\n", inl(RTC_ADDR), boot_time.us, res);
  uptime->us = this_time - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 1;
  rtc->minute = 1;
  rtc->hour   = 1;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
