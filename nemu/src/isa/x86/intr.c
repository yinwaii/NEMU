#include <cpu/exec.h>
#include "local-include/rtl.h"

#define IRQ_TIMER 32

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(s, &cpu.eflags.val);
  cpu.eflags.IF = 0;
  *t0 = cpu.CS;
  rtl_push(s, t0);
  rtl_push(s, &ret_addr);
  // cpu.eflags.TF = 0;
  // if (NO != 0x81)
  //   cpu.eflags.IF = 0;
  word_t offset_15_0 = vaddr_read(cpu.IDTR.base + NO * 8, 4) & 0x0000ffff;
  word_t offset_31_16 = vaddr_read(cpu.IDTR.base + NO * 8 + 4, 4) & 0xffff0000;
  vaddr_t target_addr = offset_15_0 | offset_31_16;
  rtl_j(s, target_addr);
  // TODO();
}

void query_intr(DecodeExecState *s) {
  if (cpu.INTR & cpu.eflags.IF) {
    cpu.INTR = false;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
  }
  // TODO();
}
