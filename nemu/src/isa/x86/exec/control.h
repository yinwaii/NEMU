#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  rtl_jr(s, ddest);

  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  // if (id_dest->width == 2)
  // {
  //   rtl_msb(s, s0, &cpu.pc, 2);
  //   rtl_push(s, s0);
  //   rtl_j(s, s->jmp_pc);
  // }
  // else
  // {
    rtl_push(s, &s->seq_pc);
    rtl_j(s, s->jmp_pc);
  // }
  // TODO();
  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
  rtl_pop(s, &cpu.pc);
  // Log("%x", cpu.pc);
  rtl_j(s, cpu.pc);
  // TODO();
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();
  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  TODO();
  print_asm("call *%s", id_dest->str);
}
