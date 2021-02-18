#include <common.h>
#include <fs.h>
#include "syscall.h"
#include <syscal.h>
#include <sys/time.h>
#include <loader.h>
#if defined(__ISA_NATIVE__)
#include <time.h>
#endif

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
    make_sys_function(open)
    make_sys_function(close)
    make_sys_function(lseek)
    make_sys_function(read)
    make_sys_function(gettimeofday)
    make_sys_function(execve)
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}

void sys_yield(Context *c)
{
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c)
{
  Log("Exit Code: %d", c->GPR2);
  // halt(c->GPR1);
  naive_uload(NULL, "/bin/menu");
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
  int res = fs_write((int)(c->GPR2), (char *)(c->GPR3), (size_t)(c->GPR4));
  c->GPRx = res;
}

void sys_brk(Context *c)
{
  c->GPRx = 0;
}

void sys_gettimeofday(Context *c)
{
  struct timeval *tv = (struct timeval *)(c->GPR2);
  struct timezone *tz = (struct timezone *)(c->GPR3);
  if (tv == NULL)
  {
    c->GPRx = -1;
    return;
  }
  #if defined(__ISA_X86__)
  uint32_t data;
  asm volatile("inl %1, %0" : "=a"(data) : "d"((uint16_t)(0x48 + 0x4)));
  tv->tv_sec = (time_t)data;
  asm volatile("inl %1, %0" : "=a"(data) : "d"((uint16_t)(0x48)));
  tv->tv_usec = (suseconds_t)data;
  #elif defined(__ISA_MIPS32__) || defined(__ISA_RISCV32__)
  tv->tv_sec = (time_t)(*(volatile uint32_t *)(0xa1000048 + 0x4));
  tv->tv_usec = (suseconds_t)(*(volatile uint32_t *)0xa1000048);
  #elif defined(__ISA_NATIVE__)
  return gettimeofday(tv, tz);
  #endif
  tz->tz_minuteswest = -8;
  tz->tz_dsttime = 0;
  c->GPRx = 0;
}

void sys_execve(Context *c)
{
  char *fname = (char *)(c->GPR2);
  // char *argv = (char *)(c->GPR3);
  // char *envp = (char *)(c->GPR4);
  Log("%s", fname);
  // halt(0);
  naive_uload(NULL, fname);
}