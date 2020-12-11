#include "common.h"

/* pa3.1
 * 2020-12-10
 * add case _EVENT_TIELD to Log message
 * FIXME: change the action later
 */
/* pa3.2
 * 2020-12-11
 * add case _EVENT_SYSCALL to call do_syscall(c)
 */
static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_YIELD: Log("yield event ID = %d", e.event); break;
    case _EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
