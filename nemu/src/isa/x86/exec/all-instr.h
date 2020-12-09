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
make_EHelper(idiv);

/* pa2.2
 * 2020-12-5
 * test code: fact.c
 */
make_EHelper(jmp);

/* pa2.2
 * 2020-12-5
 * test code: load-store.c
 */
make_EHelper(movsx);

/* pa2.2
 * 2020-12-6
 * test code: mul-longlong.c
 */
make_EHelper(imul1);

/* pa2.2
 * 2020-12-6
 * test code: recursion.c
 */
make_EHelper(call_rm);
make_EHelper(jmp_rm);

/* pa2.2
 * 2020-12-6
 * test code: shift.c
 */
make_EHelper(shr);

/* pa2.2
 * 2020-12-6
 * test code: shuixianhua.c
 */
make_EHelper(mul);

/* pa2.2
 * 2020-12-6
 * test code: sub-longlong.c
 */
make_EHelper(sbb);

/* pa2.2
 * 2020-12-6
 * test code: hello-str.c
 */
make_EHelper(neg);

/* pa2.3
 * 2020-12-9
 */
make_EHelper(out);
make_EHelper(in);