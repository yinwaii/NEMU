#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint32_t res = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return res;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int res = read(fd, buf, len);
  return (res == 0) ? 0 : 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (*w ==0 && *h ==0)
  {
    *w = screen_w;
    *h = screen_h;
    canvas_w = screen_w;
    canvas_h = screen_h;
  }
  else
  {
    canvas_w = *w;
    canvas_h = *h;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  uint32_t *p_pixels = pixels;
  int real_x = (screen_w - canvas_w) / 2 + x;
  int real_y = (screen_h - canvas_h) / 2 + y;
  for (int i = 0; i < h; i++)
  {
    int fb = open("/dev/fb", 0, 0);
    lseek(fb, (real_y + i) * screen_w + real_x, 0);
    // for (int j = 0; j < w; j++)
      // printf("x: %d y: %d p: %p realp: %p\n", j, i, pixels[i * w + j], p_pixels[j]);
    write(fb, p_pixels + y * canvas_w + x, w);
    p_pixels += canvas_w;
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  char buf[35] = "";
  int fd = open("/proc/dispinfo", 0, 0);
  int res = read(fd, buf, 30);
  sscanf(buf, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
  return 0;
}

void NDL_Quit() {
  // printf("going to quit\n");
  exit(0);
}
