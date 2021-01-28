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
  if (s->width == 2) {
    rtl_li(s, s0, reg_w(R_SP));
    rtl_li(s, s1, reg_w(R_AX));
    rtl_push(s, s1);
    rtl_li(s, s1, reg_w(R_CX));
    rtl_push(s, s1);
    rtl_li(s, s1, reg_w(R_DX));
    rtl_push(s, s1);
    rtl_li(s, s1, reg_w(R_BX));
    rtl_push(s, s1);
    rtl_push(s, s0);
    rtl_li(s, s1, reg_w(R_BP));
    rtl_push(s, s1);
    rtl_li(s, s1, reg_w(R_SI));
    rtl_push(s, s1);
    rtl_li(s, s1, reg_w(R_DI));
    rtl_push(s, s1);
  }
  else {
    rtl_mv(s, s0, &cpu.esp);
    rtl_push(s, &cpu.eax);
    rtl_push(s, &cpu.ecx);
    rtl_push(s, &cpu.edx);
    rtl_push(s, &cpu.ebx);
    rtl_push(s, s0);
    rtl_push(s, &cpu.ebp);
    rtl_push(s, &cpu.esi);
    rtl_push(s, &cpu.edi);
  }
  // TODO();
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  if (s->width == 2) {
    rtl_pop(s, s0);
    reg_l(R_DI) = *s0;
    rtl_pop(s, s0);
    reg_l(R_SI) = *s0;
    rtl_pop(s, s0);
    reg_l(R_BP) = *s0;
    rtl_pop(s, s0);
    rtl_pop(s, s0);
    reg_l(R_BX) = *s0;
    rtl_pop(s, s0);
    reg_l(R_DX) = *s0;
    rtl_pop(s, s0);
    reg_l(R_CX) = *s0;
    rtl_pop(s, s0);
    reg_l(R_AX) = *s0;
  }
  else {
    rtl_pop(s, &cpu.edi);
    rtl_pop(s, &cpu.esi);
    rtl_pop(s, &cpu.ebp);
    rtl_pop(s, s0);
    rtl_pop(s, &cpu.ebx);
    rtl_pop(s, &cpu.edx);
    rtl_pop(s, &cpu.ecx);
    rtl_pop(s, &cpu.eax);
  }
  // TODO();
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

static inline def_EHelper(movs)
{
  rtl_lm(s, s0, &cpu.esi, 0, id_src1->width);
  rtl_sm(s, &cpu.edi, 0, s0, id_dest->width);
  reg_l(R_ESI) += id_src1->width;
  reg_l(R_EDI) += id_dest->width;
  switch (id_dest->width) {
    case 1:
      print_asm("movsb");
      break;
    case 2:
      print_asm("movsw");
      break;
    case 3:
      print_asm("movsd");
      break;
    }
}