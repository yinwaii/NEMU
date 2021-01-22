static inline def_EHelper(mov) {
  // Log("Before mov: dest: %#.8x src1: %#.8x", *ddest, *dsrc1);
  operand_write(s, id_dest, dsrc1);
  // Log("After mov: dest: %#.8x src1: %#.8x", *ddest, *dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  rtl_push(s, ddest);
  // TODO();
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  TODO();
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();
  print_asm("popa");
}

static inline def_EHelper(leave) {
  rtl_mv(s, &cpu.esp, &cpu.ebp);
  rtl_pop(s, &cpu.ebp);
  // TODO();
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  // Log("%#.8x", cpu.edx);
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  // rtl_zext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, dsrc1);
  // Log("%#.8x", cpu.edx);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}
