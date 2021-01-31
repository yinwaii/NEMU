#include <NDL.h>
#include <SDL.h>
#include <sdl-debug.h>
#include <stdio.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  sdl_TODO();
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  sdl_TODO();
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  // sdl_TODO();
  char buf[20] = "", name[20] = "", ch = '\0';
  int res = NDL_PollEvent(buf, 19);
  sscanf(buf, "k%c %s\n", ch, name);
  for (int i = 0; i < ((sizeof(keyname)) / (sizeof(keyname[0]))); i++)
  {
    if (strcmp(name, keyname[i]) == 0)
      
  }
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  sdl_TODO();
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  sdl_TODO();
  return NULL;
}
