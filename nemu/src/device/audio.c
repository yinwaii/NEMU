#include <common.h>

#ifdef HAS_IOE

#include <device/map.h>
#include <SDL2/SDL.h>

#define AUDIO_PORT 0x200 // Note that this is not the standard
#define AUDIO_MMIO 0xa1000200
#define STREAM_BUF 0xa0800000
#define STREAM_BUF_MAX_SIZE 65536

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static int tail = 0;

static inline void audio_play(void *userdata, uint8_t *stream, int len) {
  // printf("into tail...\n");
  // printf("in tail: %d\n", tail);
  int nread = len;
  if (audio_base[reg_count] < len)
    nread = audio_base[reg_count];

  if (nread + tail < STREAM_BUF_MAX_SIZE)
  {
    memcpy(stream, sbuf + tail, nread);
    tail += nread;
  }
  else
  {
    int first_cpy_len = STREAM_BUF_MAX_SIZE - tail;
    memcpy(stream, sbuf + tail, first_cpy_len);
    memcpy(stream + first_cpy_len, sbuf, nread - first_cpy_len);
    tail = nread - first_cpy_len;
  }
  audio_base[reg_count] -= nread;
  if (len > nread)
    memset(stream + nread, 0, len - nread);
  // printf("tail: %d\n", tail);
  // printf("count: %d\n", audio_base[reg_count]);
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  audio_base[reg_sbuf_size] = STREAM_BUF_MAX_SIZE;
  if (audio_base[reg_init])
  {
    tail = 0;
    audio_base[reg_init] = 0;
    SDL_AudioSpec s = {};
    s.freq = audio_base[reg_freq];
    s.format = AUDIO_S16SYS;
    s.channels = audio_base[reg_channels];
    s.samples = audio_base[reg_samples];
    s.callback = audio_play;
    s.userdata = NULL;
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_OpenAudio(&s, NULL);
    SDL_PauseAudio(0);
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (void *)new_space(space_size);
  add_pio_map("audio", AUDIO_PORT, (void *)audio_base, space_size, audio_io_handler);
  add_mmio_map("audio", AUDIO_MMIO, (void *)audio_base, space_size, audio_io_handler);

  sbuf = (void *)new_space(STREAM_BUF_MAX_SIZE);
  add_mmio_map("audio-sbuf", STREAM_BUF, (void *)sbuf, STREAM_BUF_MAX_SIZE, NULL);
}
#endif	/* HAS_IOE */
