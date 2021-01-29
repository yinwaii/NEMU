#include <common.h>
#include <fs.h>
#include "syscall.h"

#define concat(a, b)       a ## b
#define make_sys_function(name) \
  case concat(SYS_, name):                \
    concat(sys_, name)(c);                \
    break;

void sys_yield(Context *c)
{
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c)
{
  Log("Exit Code: %d", c->GPR2);
  halt(c->GPR1);
  c->GPRx = 0;
}

void sys_open(Context *c)
{
  int res = fs_open((const char *)(c->GPR2), (int)(c->GPR3), (int)(c->GPR4));
  c->GPRx = res;
}

void sys_close(Context *c)
{
  int res = fs_close((int)(c->GPR2));
  c->GPRx = res;
}

void sys_read(Context *c)
{
  int res = fs_read((int)(c->GPR2), (void *)(c->GPR3), (size_t)(c->GPR4));
  c->GPRx = res;
}

void sys_lseek(Context *c)
{
  size_t res = fs_lseek((int)(c->GPR2), (size_t)(c->GPR3), (int)(c->GPR4));
  c->GPRx = res;
}

void sys_write(Context *c)
{
  int fd = c->GPR2;
  char *buf = (char *)c->GPR3;
  size_t len = (size_t)c->GPR4;
  int res = len;
  switch (fd)
  {
  case 1:
  case 2:
    // Log("Get write request!");
    for (int i = 0; i < len; i++)
    {
      putch(buf[i]);
    }
    c->GPRx = len;
    break;
  default:
    res = fs_write(fd, buf, len);
    c->GPRx = res;
    break;
  }
}

void sys_brk(Context *c)
{
  c->GPRx = 0;
}

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
    make_sys_function(open)
    make_sys_function(close)
    make_sys_function(lseek)
    make_sys_function(read)
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}