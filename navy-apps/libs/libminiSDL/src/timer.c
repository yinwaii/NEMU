#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>
#include <sdl-debug.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  sdl_TODO();
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  sdl_TODO();
  return 1;
}

uint32_t SDL_GetTicks() {
  // sdl_TODO();
  return NDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
  // sdl_TODO();
}
