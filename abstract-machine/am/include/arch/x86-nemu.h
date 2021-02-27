#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  uintptr_t this_esp;
  void *cr3;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs, eflags;
  uintptr_t esp3, ss3;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
