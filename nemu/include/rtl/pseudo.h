#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  // dest <- imm
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  const rtlreg_t full = 0xffffffff;
  rtl_xor(s, dest, src1, &full);
  // TODO();
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  rtl_sub(s, dest, rz, src1);
  // TODO();
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  const rtlreg_t self = (width < 4) ? ((1 << (width * 8)) - 1) : 0xffffffff;
  const rtlreg_t sign = 1 << (width * 8 - 1);
  if ((sign & *src1) == 0)
    *dest = self & *src1;
  else
    *dest = ~self + (self & *src1);
  // TODO();
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  const rtlreg_t self = (width < 4) ? ((1 << (width * 8)) - 1) : 0xffffffff;
  *dest = self & *src1;
  // TODO();
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  *dest = (*src1 >> (width * 8 - 1)) & 0x1;
  // FIXME
  // TODO();
}

static inline def_rtl(msbset, rtlreg_t *dest, const rtlreg_t *src1, int width)
{
  // dest <- src1[width * 8 - 1]
  *dest &= ~(0x1 << (width * 8 - 1));
  *dest |= (*src1 & 0x1) << (width * 8 - 1);
  // FIXME
  // TODO();
}

#endif
