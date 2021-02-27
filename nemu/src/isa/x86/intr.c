#include <cpu/exec.h>
#include "local-include/rtl.h"

#define IRQ_TIMER 32

vaddr_t get_ksp() {
  uint32_t idx = cpu.TR.INDEX;
  word_t offset_15_0 = vaddr_read(cpu.GDTR.base + idx * 8 + 2, 2);
  word_t offset_23_16 = vaddr_read(cpu.GDTR.base + idx * 8 + 4, 1) << 16;
  word_t offset_31_24 = vaddr_read(cpu.GDTR.base + idx * 8 + 7, 1) << 24;
  vaddr_t target_addr = offset_15_0 | offset_23_16 | offset_31_24;
  return vaddr_read(target_addr + 4, 4);
}

void set_ksp(uint32_t val) {
  uint32_t idx = cpu.TR.INDEX;
  word_t offset_15_0 = vaddr_read(cpu.GDTR.base + idx * 8 + 2, 2);
  word_t offset_23_16 = vaddr_read(cpu.GDTR.base + idx * 8 + 4, 1) << 16;
  word_t offset_31_24 = vaddr_read(cpu.GDTR.base + idx * 8 + 7, 1) << 24;
  vaddr_t target_addr = offset_15_0 | offset_23_16 | offset_31_24;
  vaddr_write(target_addr + 4, val, 4);
}

bool check_idt(uint32_t NO) {
  word_t state = vaddr_read(cpu.IDTR.base + NO * 8 + 4, 2);
  return ((state & 0x8000) != 0);
}

vaddr_t get_idt(uint32_t NO) {
  word_t offset_15_0 = vaddr_read(cpu.IDTR.base + NO * 8, 4) & 0x0000ffff;
  word_t offset_31_16 = vaddr_read(cpu.IDTR.base + NO * 8 + 4, 4) & 0xffff0000;
  vaddr_t target_addr = offset_15_0 | offset_31_16;
  return target_addr;
}

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  if (!check_idt(NO))
    return;
  if (cpu.CS.RPL == 0x3) {
    vaddr_t ksp = get_ksp();
    rtl_mv(s, t0, &cpu.esp);
    if (ksp != 0)
      rtl_li(s, &cpu.esp, ksp);
    rtl_li(s, t1, cpu.SS.val);
    rtl_push(s, t1);
    rtl_push(s, t0);
    set_ksp(0);
  }
  rtl_push(s, &cpu.eflags.val);
  cpu.eflags.IF = 0;
  rtl_li(s, t0, cpu.CS.val);
  rtl_push(s, t0);
  rtl_push(s, &ret_addr);
  // cpu.eflags.TF = 0;
  // if (NO != 0x81)
  //   cpu.eflags.IF = 0;
  rtl_j(s, get_idt(NO));
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
