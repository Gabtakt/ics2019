#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  /* pa2.1
   * 2020-12-1
   * use rtl_sm to access guest memory, to store 4 bytes data
   */
  rtl_li(&s0, 4);
  rtl_sub(&reg_l(R_ESP), &reg_l(R_ESP), &s0);
  rtl_sm(&reg_l(R_ESP), src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  /* pa2.1
   * 2020-12-2
   */
  rtl_lm(dest, &reg_l(R_ESP), 4);
  rtl_li(&s0, 4);
  rtl_add(&reg_l(R_ESP), &reg_l(R_ESP), &s0);
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  /* pa2.1
   * 2020-12-2
   * use registers: t0, t1
   */
  // OF will be set when x, y has different sign and x, (x-y) also has different sign
  t0 = (*src1) >> (width * 8 - 1) & 0x1;
  t1 = (*src2) >> (width * 8 - 1) & 0x1;
  rtl_xor(&t1, &t0, &t1);
  if (t1 == 0) {
    *dest = 0;
  }
  else {
    t1 = (*res) >> (width * 8 - 1) && 0x1;
    rtl_xor(&t1, &t0, &t1);
    *dest = t1;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  /* pa2.1
   * 2020-12-2
   */
  *dest = (*res > *src1);
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  /* pa2.2
   * 2020-12-3
   */
  t0 = (*src1) >> (width * 8 - 1) & 0x1;
  t1 = (*src2) >> (width * 8 - 1) & 0x1;
  rtl_xor(&t1, &t0, &t1);
  if (t1 == 0) {
    *dest = 0;
  }
  else {
    t1 = (*res) >> (width * 8 - 1) & 0x1;
    rtl_xor(&t1, &t0, &t1);
    *dest = t1;
  }
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  /* pa2.1
   * 2020-12-2
   */
  *dest = (*res < *src1);
}

/* pa2.1
 * 2020-12-2
 * update the define
 */
#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    reg_f(f) = *src; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = reg_f(f); \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  /* pa2.1
   * 2020-12-2
   * use register:t0
   */
  t0 = *result << (32 - width * 8);
  t0 = (t0 == 0);
  rtl_set_ZF(&t0);
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  /* pa2.1
   * 2020-12-2
   * use register:t0
   */
  t0 = *result >> (width * 8 - 1);
  t0 = (t0 & 0x1);
  rtl_set_SF(&t0);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
