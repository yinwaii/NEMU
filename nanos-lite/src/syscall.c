#include <common.h>
#include "syscall.h"

#define concat(a, b)       a ## b
#define make_sys_function(name) \
  case concat(SYS_, name):                \
    concat(sys_, name)(c);                \
    break;

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  // Log("Get syscall ID = %d", a[0]);

  switch (a[0]) {
    make_sys_function(exit)
    make_sys_function(yield)
    make_sys_function(write)
    make_sys_function(brk)
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}

void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c) {
  Log("Exit Code: %d", c->GPR2);
  halt(c->GPR1);
  c->GPRx = 0;
}

void sys_write(Context *c) {
  int fd = c->GPR2;
  char *buf = (char *)c->GPR3;
  size_t len = (size_t)c->GPR4;
  switch (fd)
  {
    case 1: case 2:
      // Log("Get write request!");
      for (int i = 0; i < len; i++)
      {
        putch(buf[i]);
      }
      c->GPRx = len;
      break;
    default:
      panic("Unhandled write request: %d", fd);
      break;
  }
}

void sys_brk(Context *c) {
  c->GPRx = 0;
}