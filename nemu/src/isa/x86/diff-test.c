#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  /* pa2.2 */
  if (ref_r->eax != reg_l(R_EAX) || ref_r->ebx != reg_l(R_EBX) || ref_r->ecx != reg_l(R_ECX) || 
		ref_r->edx != reg_l(R_EDX) || ref_r->esi != reg_l(R_ESI) ||
		ref_r->edi != reg_l(R_EDI)) {
		return false;
	}
	return true;
}

void isa_difftest_attach(void) {
}
