#include "cc.h"

static inline def_EHelper(test) {
  rtl_and(s, s0, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  rtl_set_OF(s, rz);
  // TODO();
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s, s0, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  rtl_set_OF(s, rz);
  // rtl_mv(s, ddest, s0);
  operand_write(s, id_dest, s0);
  // TODO();
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  rtl_xor(s, s0, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  rtl_set_OF(s, rz);
  // rtl_mv(s, ddest, s0);
  operand_write(s, id_dest, s0);
  // TODO();
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  // Log("Now is %#.8x, next is %#.8x, jmp is %#.8x", cpu.pc, s->seq_pc, s->jmp_pc);
  rtl_or(s, s0, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  rtl_set_OF(s, rz);
  // rtl_mv(s, ddest, s0);
  operand_write(s, id_dest, s0);
  // TODO();
  print_asm_template2(or);
}

static inline def_EHelper(not) {
  rtl_not(s, s0, ddest);
  // Log("not %#.8x = %#.8x", *ddest, *s0);
  operand_write(s, id_dest, s0);
  // TODO();
  print_asm_template1(not);
}

static inline def_EHelper(sar) {
  rtl_sar(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  // TODO();
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  // TODO();
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  rtl_shr(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  operand_write(s, id_dest, s0);
  // TODO();
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}


static inline def_EHelper(setcc) {
  // Log("%#.8x", cpu.edx);
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);
  // Log("%#.8x", cpu.edx);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}
