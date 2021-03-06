#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  /* pa2.2
   * 2020-12-4
   * use registers: s0, s1
   */
  rtl_and(&s0, &id_dest->val, &id_src->val);
  // update ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);
  // set OF and CF to zero
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(test);
}

make_EHelper(and) {
  /* pa2.2
   * 2020-12-3
   * use registers: s0
   */
  rtl_and(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // update ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);
  // set OF and CF to zero
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(and);
}

make_EHelper(xor) {
  /* pa2.1
   * 2020-12-2
   * use registers: s0
   */
  rtl_xor(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // updat ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(xor);
  // set OF and CF to zero
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(xor);
}

make_EHelper(or) {
  /* pa2.2
   * 2020-12-4
   * use registers: 
   */
  rtl_or(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // updat ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(xor);
  // set OF and CF to zero
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(or);
}

make_EHelper(sar) {
  /* pa2.2
   * 2020-12-5
   * use registers: s0
   * NOTE: unnecessary to update CF and OF in NEMU
   */
  rtl_sar(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // update ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
   /* pa2.2
   * 2020-12-5
   * use registers: s0
   * NOTE: unnecessary to update CF and OF in NEMU
   */
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // update ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
   /* pa2.2
   * 2020-12-6
   * use registers: s0
   * NOTE: unnecessary to update CF and OF in NEMU
   */
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // update ZF and SF
  rtl_update_ZFSF(&s0, id_dest->width);
  
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  /* pa2.2
   * 2020-12-5
   * NOTE: not instruction does not change any FLAGS
   * use registers: s0
   */
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);

  print_asm_template1(not);
}

make_EHelper(rol) {
  /* Pa2.3
   * 2020-12-9
   * NOTE: ring shift left , when only shift one bit
   *                          Operation
      temp ← COUNT;
      WHILE (temp ≠ 0)
      DO
      tmpcf ← high-order bit of (r/m);
      r/m ← r/m * 2 + (tmpcf);
      temp ← temp - 1;
      OD;
      IF COUNT = 1
      THEN
      IF high-order bit of r/m ≠ CF
      THEN OF ← 1;
      ELSE OF ← 0;
      FI;
      ELSE OF ← undefined;
      FI;
   * use registers: s0, s1
   */
	rtl_mv(&s0, &id_dest->val);
  int i = 0;
	for( ; i < id_src->val; i++) {
		rtl_msb(&s1, &s0, id_dest->width);
		s0 = s0 << 1;
		s0 |= s1;
		rtl_set_CF(&s1);
	}
  operand_write(id_dest, &s0);
	if(id_src->val == 1) {
    rtl_msb(&s0, &s0, id_dest->width);
    s0 ^= s1;
    rtl_set_CF(&s0);
  }
	
	print_asm_template2(rol);
}