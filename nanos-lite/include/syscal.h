#ifndef __SYSCAL_H__
#define __SYSCAL_H__
#include <common.h>

#define concat(a, b) a ## b
#define make_sys_declare(name) void concat(sys_, name)(Context * c);

make_sys_declare(exit)
make_sys_declare(yield)
make_sys_declare(write)
make_sys_declare(brk)
make_sys_declare(open)
make_sys_declare(close)
make_sys_declare(lseek)
make_sys_declare(read)
make_sys_declare(gettimeofday)

#endif