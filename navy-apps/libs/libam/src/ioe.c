#include <am.h>
#include <klib-macros.h>
#include <stdio.h>
#define concat(a, b) a ## b
#define reg_map_read(f)                                        \
  f(TIMER_CONFIG) f(TIMER_RTC) f(TIMER_UPTIME) f(INPUT_CONFIG) \
      f(INPUT_KEYBRD) f(GPU_CONFIG) f(GPU_STATUS)              \
          f(UART_CONFIG) f(AUDIO_CONFIG) f(AUDIO_STATUS)       \
              f(DISK_CONFIG) f(NET_CONFIG)
#define reg_map_write(f) \
  f(GPU_FBDRAW) f(AUDIO_CTRL) f(AUDIO_PLAY)
#define make_map(name) \
  , [CONCAT(AM_, name)] = "/dev/am/AM_" TOSTRING(name)

bool ioe_init() {
  return true;
}

char am_filemap[30][30] = {
    [0] = "/dev/am" reg_map_read(make_map)
        reg_map_write(make_map)};

void ioe_read(int reg, void *buf) {
  // printf("Now open: %s\n", am_filemap[reg]);
  int fd = open(am_filemap[reg], 0, 0);
  read(fd, buf, 0);
}
void ioe_write(int reg, void *buf) {
  int fd = open(am_filemap[reg], 0, 0);
  write(fd, buf, 0);
}
