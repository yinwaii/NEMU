#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

#define check_register(reg)                                       \
  if (ref_r->reg != cpu.reg)                                      \
  {                                                               \
    Log("Different "str(reg)": QEMU: %#.8x NEMU: %#.8x", ref_r->reg, cpu.reg); \
    return false;                                                 \
  }                                                               \
  else

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // Log("QEMU: %#.8x NEMU: %#.8x", ref_r->ebp, cpu.ebp);
  check_register(eax)
  check_register(ecx)
  check_register(edx)
  check_register(ebx)
  check_register(esp)
  check_register(ebp)
  check_register(esi)
  check_register(edi)
  // check_register(eflags.CF)
  // check_register(eflags.ZF)
  // check_register(eflags.SF)
  // check_register(eflags.OF)
    return true;
}

void isa_difftest_attach() {
}
