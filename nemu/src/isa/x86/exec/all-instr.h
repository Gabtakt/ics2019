#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

/* pa2.1
 * 2020-12-2
 * test code: dummy.c
 */
make_EHelper(call);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);

/* pa2.2
 * 2020-12-3
 * test code: add-longlong.c
 */
make_EHelper(lea);
make_EHelper(and);
make_EHelper(nop);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(or);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(leave);
make_EHelper(inc);
make_EHelper(cmp);
make_EHelper(pop);

/* pa2.2
 * 2020-12-5
 * test code: bit.c
 */
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(dec);
make_EHelper(not);

/* pa2.2
 * 2020-12-5
 * test code: div.c
 */
make_EHelper(imul2);
make_EHelper(cltd);