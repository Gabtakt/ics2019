#include "cpu/exec.h"

make_EHelper(lidt) {
  /* pa3.1
   * 2020-12-10
   * set a instruction's length and base addr in IDTR
   * INSTRUCITON LIDT
   *  IF OperandSize = 16
       THEN IDTR.Limit:Base ← m16:24 (* 24 bits of base loaded *)
       ELSE IDTR.Limit:Base ← m16:32
      FI;
   */
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  if (decinfo.isa.is_operand_size_16) {
    cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4) && 0x00ffffff;
  }
  else {
    cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  /* pa3.1
   * 2020-12-10
   */
  raise_intr(id_dest->val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}


make_EHelper(iret) {
  /* pa3.1
   * 2020-12-10
   * pop pc, cs, eflags from stack, then jump tp target
   */
  rtl_pop(&decinfo.jmp_pc);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags_val);
  decinfo_set_jmp(true);
  rtl_j(decinfo.jmp_pc);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  /* pa2.3
   * 2020-12-9
   */
  switch(id_src->width) {
      case 1:
        s0 = pio_read_b(id_src->val);
        break;
      case 2:
        s0 = pio_read_w(id_src->val);
        break;
      case 4:
        s0 = pio_read_l(id_src->val);
        break;
      default :
        assert(0);
        break;
  }
  operand_write(id_dest,&s0);

  print_asm_template2(in);
}

make_EHelper(out) {
  /* pa2.3
   * 2020-12-9
   */
  switch (id_src->width) {
  case 1:
    pio_write_b(id_dest->val, id_src->val);
    break;
  case 2:
    pio_write_w(id_dest->val, id_src->val);
    break;
  case 4:
    pio_write_l(id_dest->val, id_src->val);
    break;
  default :
    assert(0);
    break;
  }
  print_asm_template2(out);
}
