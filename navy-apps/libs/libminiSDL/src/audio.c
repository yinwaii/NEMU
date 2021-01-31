#include <NDL.h>
#include <SDL.h>
#include <sdl-debug.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  // sdl_TODO();
  return 0;
}

void SDL_CloseAudio() {
  sdl_TODO();
}

void SDL_PauseAudio(int pause_on) {
  sdl_TODO();
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  sdl_TODO();
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  // sdl_TODO();
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  sdl_TODO();
}

void SDL_LockAudio() {
  sdl_TODO();
}

void SDL_UnlockAudio() {
  sdl_TODO();
}
