#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

/* pa2.1
 * 2020-12-2
 */
make_EHelper(call);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);

/* pa2.2
 * begin: 2020-12-3
 * end:
 */
make_EHelper(lea);