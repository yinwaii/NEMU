#include <am.h>
#include <nemu.h>
#include <klib.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static int head = 0;

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_COUNT_ADDR, 0);
  outl(AUDIO_INIT_ADDR, 1);
  head = 0;
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  // printf("into head...\n");
  int len = ctl->buf.end - ctl->buf.start;
  int buf_sizemax = inl(AUDIO_SBUF_SIZE_ADDR);
  int buf_count = inl(AUDIO_COUNT_ADDR);
  int free = buf_sizemax - buf_count;
  int nwrite = len;
  assert(len <= buf_sizemax);
  while (buf_sizemax - buf_count < len)
    ;
  if (free < len)
    nwrite = free;
  if (nwrite + head < buf_sizemax)
  {
    memcpy((void *)(AUDIO_SBUF_ADDR + head), ctl->buf.start, nwrite);
    head += nwrite;
  }
  else
  {
    int first_cpy_len = buf_sizemax- head;
    memcpy((void *)(AUDIO_SBUF_ADDR + head), ctl->buf.start, first_cpy_len);
    memcpy((void *)AUDIO_SBUF_ADDR, ctl->buf.start + first_cpy_len, nwrite - first_cpy_len);
    head = nwrite - first_cpy_len;
  }
  buf_count += nwrite;
  outl(AUDIO_COUNT_ADDR, buf_count);
  // printf("head: %d\n", head);
  // printf("count: %d\n", buf_count);
}
