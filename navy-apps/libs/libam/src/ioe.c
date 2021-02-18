#include <am.h>
#include <klib-macros.h>
#include <string.h>
#define concat(a, b) a ## b
#define reg_map(f) \
  f(AM_TIMER_CONFIG) f(AM_TIMER_RTC) f(AM_TIMER_UPTIME) f(AM_INPUT_CONFIG) \
  f(AM_INPUT_KEYBRD) f(AM_GPU_CONFIG) f(AM_GPU_FBDRAW) f(AM_GPU_STATUS) \
  f(AM_UART_CONFIG) f(AM_AUDIO_CONFIG) f(AM_AUDIO_CTRL) f(AM_AUDIO_STATUS) \
  f(AM_AUDIO_PLAY) f(AM_DISK_CONFIG) f(AM_NET_CONFIG)
#define var_reg(name) \
  concat(name, _T) concat(VAR_, name);
#define case_state_read(name)            \
  case name:                             \
    concat(VAR_, name) = io_read(name);   \
    memcpy(buf, (void *)&concat(VAR_, name), sizeof(concat(VAR_, name))); \
    break;

reg_map(var_reg)

bool ioe_init() {
  return true;
}

void ioe_read (int reg, void *buf) {
  switch(reg) {
    reg_map(case_state_read)
  }
}
void ioe_write(int reg, void *buf) {
  switch(reg) {
    case AM_GPU_FBDRAW:
      memcpy((void *)&VAR_AM_GPU_FBDRAW, buf, sizeof(VAR_AM_GPU_FBDRAW));
      io_write(AM_GPU_FBDRAW, VAR_AM_GPU_FBDRAW.x, VAR_AM_GPU_FBDRAW.y, VAR_AM_GPU_FBDRAW.pixels, VAR_AM_GPU_FBDRAW.w, VAR_AM_GPU_FBDRAW.h, VAR_AM_GPU_FBDRAW.sync);
      break;
    case AM_AUDIO_CTRL:
      memcpy((void *)&VAR_AM_AUDIO_CTRL, buf, sizeof(VAR_AM_AUDIO_CTRL));
      io_write(AM_AUDIO_CTRL, VAR_AM_AUDIO_CTRL.freq, VAR_AM_AUDIO_CTRL.channels, VAR_AM_AUDIO_CTRL.samples);
      break;
    case AM_AUDIO_PLAY:
      memcpy((void *)&VAR_AM_AUDIO_PLAY, buf, sizeof(VAR_AM_AUDIO_PLAY));
      io_write(AM_AUDIO_PLAY, VAR_AM_AUDIO_PLAY.buf);
      break;
    }
}
