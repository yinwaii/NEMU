#include <isa.h>
#include <monitor/difftest.h>
#include <memory/paddr.h>
#include "../local-include/reg.h"
#include "difftest.h"

#define check_register(reg)                                       \
  if (ref_r->reg != cpu.reg)                                      \
  {                                                               \
    Log("Different "str(reg)": QEMU: %#.8x NEMU: %#.8x, pc: %#.8x", ref_r->reg, cpu.reg, cpu.pc);  \
    return false;                                                 \
  }                                                               \
  else
#define inno_register(reg)                                                      \
  if (ref_r->reg != cpu.reg)                                                     \
  {                                                                              \
    Log("Different " str(reg) ": QEMU: %#.8x NEMU: %#.8x, pc: %#.8x", ref_r->reg, cpu.reg, cpu.pc); \
  }                                                                              

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // Log("QEMU: %#.8x NEMU: %#.8x, when pc is %#.8x", ref_r->ebp, cpu.ebp, cpu.pc);
  // Log("QEMU: %#.8x NEMU: %#.8x, when pc is %#.8x", ref_r->eax, cpu.eax, cpu.pc);
  // inno_register(eflags.CF)
  // inno_register(eflags.ZF)
  // inno_register(eflags.SF)
  // inno_register(eflags.OF)
  check_register(eax)
  check_register(ecx)
  check_register(edx)
  check_register(ebx)
  check_register(esp)
  check_register(ebp)
  check_register(esi)
  check_register(edi)
  check_register(pc)
    return true;
}

void isa_difftest_attach() {
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  CPU_state ref_cpu;
  ref_cpu.eax = 0x7e00;
  ref_cpu.pc = 0x7e40;
  ref_cpu.esp = 0x8000;
  uint16_t idt_data[3] = { cpu.IDTR.limit, cpu.IDTR.base, ((uint32_t)cpu.IDTR.base) >> 16 };
  char attach_instructions[] = {0x0f, 0x01, 0x18, 0x9d};
  ref_difftest_memcpy_from_dut(0x7e00, idt_data, sizeof(idt_data));
  ref_difftest_memcpy_from_dut(0x7e40, attach_instructions, sizeof(attach_instructions));
  ref_difftest_memcpy_from_dut(0x8000, &cpu.eflags.val, 4);
  ref_difftest_setregs(&ref_cpu);
  ref_difftest_exec(2);
  ref_difftest_setregs(&cpu);
}
