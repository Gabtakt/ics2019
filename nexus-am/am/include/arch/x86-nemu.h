#ifndef __ARCH_H__
#define __ARCH_H__

/* pa3.1
 * 2020-12-10
 * Reorganize the Context:
 *   ESP(paramater)
 *   $0
 *   EDI
 *   ESI
 *   EBP
 *   ESP
 *   EBX
 *   EDX
 *   ECX
 *   EAX
 *   IRQ
 *   EIP(PC)
 *   CS
 *   EFLAGS
 * NOTE: the oder depends on instruction 'int', it push elflags, cs, eip(pc),
 * in nexus-am/am/src/x86/nemu/trap.S, first push irq, then call pusha to
 *  push all registers.
 */
struct _Context {
  // uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
  // struct _AddressSpace *as;
  // int irq;
  struct _AddressSpace *as;
  uintptr_t edi;
  uintptr_t esi;
  uintptr_t ebp;
  uintptr_t esp;
  uintptr_t ebx;
  uintptr_t edx;
  uintptr_t ecx;
  uintptr_t eax;
  int irq;
  uintptr_t eip;
  uintptr_t cs;
  uintptr_t eflags;

};

/* pa3.2
 * 2020-12-11
 */
#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
