#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  /* pa2.2 */
  if (ref_r->eax != reg_l(R_EAX) || ref_r->ebx != reg_l(R_EBX) || ref_r->ecx != reg_l(R_ECX) || 
		ref_r->edx != reg_l(R_EDX) || ref_r->ebp != reg_l(R_EBP) || ref_r->esi != reg_l(R_ESI) ||
		ref_r->edi != reg_l(R_EDI) || ref_r->pc != cpu.pc) {
		return false;
	}
  // // magic number: 0x000000C0 == 1100 0000 == mark 'ZF' and 'SF'
  // // NOTE: ignord to compare 'IF', 'CF', 'OF' here(NEMU has different behavor with QEMU)
  // uint32_t QEMU_eflags = ref_r->eflags & 0x000000C0;
  // uint32_t NEMU_eflags = cpu.eflags_val & 0x000000C0;
  // if (QEMU_eflags != NEMU_eflags) {
  //   return false;
  // }
	return true;
}

  #define ISA_DIFF_ST2 0x1000000
  #define ISA_DIFF_END2 PMEM_SIZE
void isa_difftest_attach(void) {
	/* pa3.3
	 * 2020-12-14
   * MEM_REF[0x7c00 - 1, 0] <- MEM_DUT[0x7c00 - 1, 0]
   * MEM_REF[PMEM_SIZE - 1, 0x100000] <- MEM_DUT[PMEM_SIZE - 1, 0x100000]
   * QEMU_REG_STATE <- NEMU_REG_STATE
   * API: nemu/tools/qemu-diff/src/diff-test.c
	 */
	ref_difftest_memcpy_from_dut(0, &pmem[0], 0x7c00);
	ref_difftest_memcpy_from_dut(0x100000, &pmem[0x100000], PMEM_SIZE - 0x100000);
	ref_difftest_setregs(&cpu);

  // printf("begian@|");
  // char *mainargs = guest_to_host(0);
  // printf("@|");
  // ref_difftest_memcpy_from_dut(PC_START + IMAGE_START, mainargs, 0x7c00);
  // printf("@@|");
  // mainargs = (char *)ISA_DIFF_ST2;
  // ref_difftest_memcpy_from_dut(PC_START + IMAGE_START + ISA_DIFF_ST2, mainargs, ISA_DIFF_END2 - ISA_DIFF_ST2);
  
  // printf("@@@|");
  // ref_difftest_setregs(&cpu);

  // printf("@@@@|");

  /* QEMU_IDTR <- NEMU_IDTR
   * MEM_REF[0x7e05, 0x7e00] <- idtr.base | idtr.limit
   * MEM_REF[_, 0x7e40] <- lidt (0x7e00)
   * QEMU_EIP <- 0x7e40
   * QEMU execute once
   * API: nemu/tools/qemu-diff/src/diff-test.c
   * let QEMU call lidt to implement it
   */
  // uint16_t limit = cpu.idtr.limit;
  // uint32_t base = cpu.idtr.base;
  // ref_difftest_memcpy_from_dut(0x7e00, (void *)&limit, sizeof(limit));
  // ref_difftest_memcpy_from_dut(0x7e00 + sizeof(limit), (void *)&base, sizeof(base));
  // // the byte ocde of lidt (0x7e00)
  // uint8_t lidt[] = { 0x0f, 0x01, 0x1d, 0x00, 0x7e, 0x00, 0x00 };
  // ref_difftest_memcpy_from_dut(0x7e40, (void *)&lidt, sizeof(lidt));
  // CPU_state cpu2 = cpu;
  // cpu2.pc = 0x7e40;
	// ref_difftest_setregs(&cpu2);
  // difftest_exec(1);
}
