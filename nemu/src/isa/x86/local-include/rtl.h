#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  rtl_subi(s, &cpu.esp, &cpu.esp, (sword_t)4);
  rtl_sm(s, &cpu.esp, PMEM_BASE, src1, 4);
  // TODO();
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(s, dest, &cpu.esp, PMEM_BASE, 4);
  rtl_addi(s, &cpu.esp, &cpu.esp, 4);
  // TODO();
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  const rtlreg_t sign = 1 << (width * 8 - 1);
  if (((*res & sign) == (*src2 & sign)) && ((*src1 & sign) != (*src2 & sign)))
    *dest = 1;
  else
    *dest = 0;
  // TODO();
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  rtl_setrelop(s, RELOP_LTU, dest, src1, res);
  // Log("%#.8x %#.8x %#.8x", *dest, *src1, *res);
  // TODO();
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  const rtlreg_t sign = 1 << (width * 8 - 1);
  if (((*res & sign) != (*src2 & sign)) && ((*src1 & sign) == (*src2 & sign)))
    *dest = 1;
  else
    *dest = 0;
  // TODO();
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  rtl_setrelop(s, RELOP_GTU, dest, src1, res);
  // TODO();
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.eflags.f = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = cpu.eflags.f; \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  const rtlreg_t self = (width < 4) ? ((1 << (width * 8)) - 1) : 0xffffffff;
  if ((*result & self) == 0)
    cpu.eflags.ZF = 1;
  else
    cpu.eflags.ZF = 0;
  // TODO();
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  const rtlreg_t sign = 1 << (width * 8 - 1);
  if ((*result & sign) == 0)
    cpu.eflags.SF = 0;
  else
    cpu.eflags.SF = 1;
  // TODO();
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
