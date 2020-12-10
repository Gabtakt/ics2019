#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  /* pa2.1
   * 2020-12-1
   * connect: x86/decode.c make_DHelper(push)
   * use rtl_push() to push value into memory
   */
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  /* pa2.2
   * 2020-12-5
   */
  rtl_pop(&s0);
  operand_write(id_dest, &s0);
  // rtl_pop(&id_dest->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  /* pa3.1
   * 2020-12-10
   * push eax, ecx, edx, ebx, esp, ebp, esi, edi
   * NOTE: before push all, should save $esp (because push 'x' will change esp)
   */
  rtl_mv(&s0, &reg_l(R_ESP));
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
  // push $esp
  rtl_push(&s0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  /* pa2.2
   * 2020-12-4
   * NOTE: do not check isa_oprand_size_16, because NEMU is
   * a 32-bit emulator.
   */
  rtl_mv(&reg_l(R_ESP), &reg_l(R_EBP));
  rtl_pop(&reg_l(R_EBP));

  print_asm("leave");
}

make_EHelper(cltd) {
  /* pa2.2
   * 2020-12-5
   * use registers: s0
   */
  if (decinfo.isa.is_operand_size_16) {
    rtl_sext(&s0, &reg_l(R_EAX), 2);
    rtl_sari(&s0, &s0, 0xf);
    rtl_sr(R_DX, &s0, 2);
  }
  else {
    rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 0x1f);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  /* pa2.3
   * 2020-12-9
   * */
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AX, 1);
    rtl_sext(&s0, &s0, 1);
    rtl_sr(R_AX, &s0, 1);
  }
  else {
    rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
