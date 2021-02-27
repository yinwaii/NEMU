#include <monitor/difftest.h>
#include <memory/paddr.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);
void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr);

static inline def_EHelper(lidt) {
  rtl_lm(s, s0, s->isa.mbase, s->isa.moff, 2);
  cpu.IDTR.limit = *s0;
  rtl_lm(s, s1, s->isa.mbase, s->isa.moff + 2, 4);
  if (id_dest->width == 2)
    cpu.IDTR.base = *s1 & 0x00ffffff;
  else
    cpu.IDTR.base = *s1;
  // TODO();
  print_asm_template1(lidt);
}

static inline def_EHelper(lgdt) {
  rtl_lm(s, s0, s->isa.mbase, s->isa.moff, 2);
  cpu.GDTR.limit = *s0;
  rtl_lm(s, s1, s->isa.mbase, s->isa.moff + 2, 4);
  if (id_dest->width == 2)
    cpu.GDTR.base = *s1 & 0x00ffffff;
  else
    cpu.GDTR.base = *s1;
  // TODO();
  print_asm_template1(lgdt);
}

static inline def_EHelper(ltr)
{
  cpu.TR.val = *ddest;
  // TODO();
  print_asm_template1(ltr);
}

static inline def_EHelper(mov_r2cr) {
  switch (id_dest->reg)
  {
    case 0:
      rtl_mv(s, &cpu.CR0.val, dsrc1);
      break;
    case 3:
      rtl_mv(s, &cpu.CR3.val, dsrc1);
      break;
    default:
      panic("Use the CR%d register which should not be used now!", id_dest->reg);
    }
  // TODO();
  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  switch (id_src1->reg)
  {
    case 0:
      rtl_mv(s, ddest, &cpu.CR0.val);
      break;
    case 3:
      rtl_mv(s, ddest, &cpu.CR3.val);
      break;
    default:
      panic("Use the CR%d register which should not be used now!", id_dest->reg);
    }
  // TODO();
  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  switch (s->opcode)
  {
  case 0xcc:
    raise_intr(s, (uint32_t)0x3, s->seq_pc);
    break;
  case 0xcd:
    raise_intr(s, *ddest, s->seq_pc);
    break;
  case 0xce:
    raise_intr(s, (uint32_t)0x4, s->seq_pc);
    break;
  default:
    printf("\33[1;31m call int with %#.2x! \33[0m", s->opcode);
    break;
  }
  // TODO();
  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

void set_ksp(uint32_t val);
static inline def_EHelper(iret) {
  rtl_pop(s, s0);
  rtl_pop(s, s1);
  cpu.CS.val = *s1;
  rtl_pop(s, &cpu.eflags.val);
  rtl_jr(s, s0);
  if (cpu.CS.RPL == 0x3) {
    rtl_pop(s, s0);
    rtl_pop(s, s1);
    cpu.SS.val = *s1;
    set_ksp(cpu.esp);
    rtl_mv(s, &cpu.esp, s0);
  }
  // TODO();
  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  switch(id_dest->width) {
    case 4:
      rtl_li(s, s0, pio_read_l(*dsrc1));
      break;
    case 2:
      rtl_li(s, s0, pio_read_w(*dsrc1));
      break;
    case 1:
      rtl_li(s, s0, pio_read_b(*dsrc1));
    }
  operand_write(s, id_dest, s0);
  // TODO();
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  switch(id_dest->width) {
    case 4:
      pio_write_l(*ddest, *dsrc1);
      break;
    case 2:
      pio_write_w(*ddest, *dsrc1);
      break;
    case 1:
      pio_write_b(*ddest, *dsrc1);
      break;
    }
  // TODO();
  print_asm_template2(out);
}
