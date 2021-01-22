#include "cc.h"

static inline def_EHelper(test) {
  TODO();
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
  TODO();
  print_asm_template2(or);
}

static inline def_EHelper(not) {
  TODO();
  print_asm_template1(not);
}

static inline def_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  TODO();
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
