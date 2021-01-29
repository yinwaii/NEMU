#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

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

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
