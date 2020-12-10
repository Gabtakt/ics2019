#include "rtl/rtl.h"
#include "cpu/exec.h"
#include "isa/mmu.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* pa3.1
   * 2020-12-10
   * Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags_val);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  cpu.eflags.IF = 0;
  vaddr_t intr_base = cpu.idtr.base + sizeof(GateDesc) * NO;
  vaddr_t lo = vaddr_read(intr_base, 4) & 0x0000ffff;
  vaddr_t hi = vaddr_read(intr_base + 4, 4) & 0xffff0000;
  decinfo.jmp_pc = hi | lo;
  decinfo_set_jmp(true);
  rtl_j(decinfo.jmp_pc);
}

bool isa_query_intr(void) {
  return false;
}
