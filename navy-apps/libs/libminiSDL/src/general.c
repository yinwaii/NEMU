#include <NDL.h>
#include <sdl-debug.h>

int SDL_Init(uint32_t flags) {
  // TODO();
  return NDL_Init(flags);
}

void SDL_Quit() {
  // TODO();
  NDL_Quit();
}

char *SDL_GetError() {
  // TODO();
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  // sdl_TODO();
  return -1;
}

int SDL_ShowCursor(int toggle) {
  sdl_TODO();
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
  sdl_TODO();
}
