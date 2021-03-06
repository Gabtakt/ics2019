#ifndef __X86_REG_H__
#define __X86_REG_H__

#include "common.h"

#define PC_START IMAGE_START

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* pa1.0
 * 2020-11-03
 * change the struct of GPR's
 */
typedef struct {
  union {
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];

    /* Do NOT change the order of the GPRs' definitions. */

    /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
     * in PA2 able to directly access these registers.
     */
    struct {
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };
  // Do NOT fucking change the order of pc!
  vaddr_t pc;

  /* pa2.1
   * 2020-12-2
   * EFLAGS
   * Bit  Name   Function
     0   CF     Carry Flag -- Set on high-order bit carry or borrow; cleared
                 otherwise.
     6   ZF     Zero Flag -- Set if result is zero; cleared otherwise.
     7   SF     Sign Flag -- Set equal to high-order bit of result (0 is
                 positive, 1 if negative).
     9   IF     Interrupt Enable -- Setting IF allows the CPU to recognize
                 external (maskable) interrupt requests. Clearing IF disables
                 these interrupts.
     11  OF     Overflow Flag -- Set if result is too large a positive number
                 or too small a negative number (excluding sign-bit) to fit in
                 destination operand; cleared otherwise.            
   * 
   */
  union {
    struct {
      uint32_t CF : 1;
      uint32_t : 5;
      uint32_t ZF : 1;
      uint32_t SF : 1;
      uint32_t : 1;
      uint32_t IF : 1;
      uint32_t : 1;
      uint32_t OF : 1;
    } EFLAGS;
    uint32_t eflags_val;
  } ;
  
} CPU_state;



static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])
/* pa2.1 
 * 2020-12-2
 * add a diefine of reg_f(index)
 */
#define reg_f(name) (cpu.EFLAGS.name)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  extern const char* regsw[];
  extern const char* regsb[];
  assert(index >= 0 && index < 8);

  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
