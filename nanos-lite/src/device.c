#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

#define reg_map_read(f)                                        \
  f(TIMER_CONFIG) f(TIMER_RTC) f(TIMER_UPTIME) f(INPUT_CONFIG) \
      f(INPUT_KEYBRD) f(GPU_CONFIG) f(GPU_STATUS)              \
          f(UART_CONFIG) f(AUDIO_CONFIG) f(AUDIO_STATUS)       \
              f(DISK_CONFIG) f(NET_CONFIG)
#define reg_map_write(f) \
  f(GPU_FBDRAW) f(AUDIO_CTRL) f(AUDIO_PLAY)
#define make_function_read(name)                                          \
  size_t CONCAT(name, _read)(void *buf, size_t offset, size_t len)  \
  {                                                                       \
    CONCAT(name, _T)                                         \
    CONCAT(VAR_, name) = io_read(name);                      \
    memcpy(buf, (void *)&CONCAT(VAR_, name), sizeof(CONCAT(VAR_, name))); \
    return sizeof(CONCAT(VAR_, name));                                    \
  }
#define function_read(name) make_function_read(CONCAT(AM_, name))

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *p_buf = (char *)buf;
  for (size_t i = 0; i < len; i++)
  {
    putch(*p_buf);
    p_buf++;
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  int size = 1;
  char *p_buf = (char *)buf;
  strcpy(p_buf, "");
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  char str[15] = "kd ";
  if (ev.keycode == AM_KEY_NONE)
    return 0;
  if (!ev.keydown)
    str[1] = 'u';
  strcat(str, keyname[ev.keycode]);
  strcat(str, "\n");
  int tmp_len = strlen(str);
  if (size + tmp_len > len)
  {
    panic("Too short length: %d for events, %d needed", len, size + tmp_len);
    return 0;
  }
  strcpy(p_buf, str);
  p_buf += tmp_len;
  size += tmp_len;
  return size;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  char str[30];
  char *p_buf = (char *)buf;
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  sprintf(str, "WIDTH: %d\nHEIGHT: %d", config.width, config.height);
  int size = strlen(str) + 1;
  if (size > len)
  {
    panic("Too short length: %d for dispinfo, %d needed!", len, size);
    return 0;
  }
  strcpy(p_buf, str);
  return size;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  uint32_t *p_buf = (uint32_t *)buf;
  for (int i = 0; i < len; i++)
  {
    io_write(AM_GPU_FBDRAW, (offset + i) % config.width, (offset + i) / config.width, (void *)(p_buf + i), 1, 1, true);
  }
  return len;
}

reg_map_read(function_read)

size_t AM_GPU_FBDRAW_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_FBDRAW_T VAR_AM_GPU_FBDRAW;
  memcpy((void *)&VAR_AM_GPU_FBDRAW, buf, sizeof(VAR_AM_GPU_FBDRAW));
  io_write(AM_GPU_FBDRAW, VAR_AM_GPU_FBDRAW.x, VAR_AM_GPU_FBDRAW.y, VAR_AM_GPU_FBDRAW.pixels, VAR_AM_GPU_FBDRAW.w, VAR_AM_GPU_FBDRAW.h, VAR_AM_GPU_FBDRAW.sync);
  return sizeof(VAR_AM_GPU_FBDRAW);
}

size_t AM_AUDIO_CTRL_write(const void *buf, size_t offset, size_t len) {
  AM_AUDIO_CTRL_T VAR_AM_AUDIO_CTRL;
  memcpy((void *)&VAR_AM_AUDIO_CTRL, buf, sizeof(VAR_AM_AUDIO_CTRL));
  io_write(AM_AUDIO_CTRL, VAR_AM_AUDIO_CTRL.freq, VAR_AM_AUDIO_CTRL.channels, VAR_AM_AUDIO_CTRL.samples);
  return sizeof(VAR_AM_AUDIO_CTRL);
}

size_t AM_AUDIO_PLAY_write(const void *buf, size_t offset, size_t len) {
  AM_AUDIO_PLAY_T VAR_AM_AUDIO_PLAY;
  memcpy((void *)&VAR_AM_AUDIO_PLAY, buf, sizeof(VAR_AM_AUDIO_PLAY));
  io_write(AM_AUDIO_PLAY, VAR_AM_AUDIO_PLAY.buf);
  return sizeof(VAR_AM_AUDIO_PLAY);
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
  // Log("Initialization of devices finished.");
}
