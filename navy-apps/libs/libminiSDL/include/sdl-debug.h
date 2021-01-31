#ifndef __SDL_DEBUG_H__
#define __SDL_DEBUG_H__
#include <assert.h>
#include <stdio.h>
#define str(s) # s
#define sdl_TODO() printf("The function %s in file %s has not implemented!\n", __FUNCTION__, __FILE__); assert(0)
#endif