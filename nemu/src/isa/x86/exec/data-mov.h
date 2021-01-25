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
  rtl_pop(s, ddest);
  // TODO();
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
    rtl_lr(s, s0, R_AX, 2);
    rtl_sext(s, s0, s0, 2);
    rtl_sari(s, s0, s0, 16);
    rtl_sr(s, R_DX, s0, 2);
    // TODO();
  }
  else {
    rtl_lr(s, s0, R_EAX, 4);
    rtl_sari(s, s0, s0, 31);
    rtl_sari(s, s0, s0, 1);
    rtl_sr(s, R_EDX, s0, 4);
    // TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    rtl_lr(s, s0, R_AL, 1);
    rtl_sext(s, s0, s0, 1);
    rtl_sr(s, R_AX, s0, 2);
    // TODO();
  }
  else {
    rtl_lr(s, s0, R_AX, 2);
    rtl_sext(s, s0, s0, 2);
    rtl_sr(s, R_EAX, s0, 4);
    // TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  // Log("Before mov: dest: %#.8x src1: %#.8x", *ddest, *dsrc1);
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  // Log("After mov: dest: %#.8x src1: %#.8x", *ddest, *dsrc1);
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
