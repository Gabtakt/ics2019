#include "common.h"
#include "syscall.h"

int sys_yield();
void sys_exit(uintptr_t arg);
int sys_write(int fd, const void *buf, size_t len);

/* pa3.2
 * 2020-12-11
 */
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_yield: c->GPRx = sys_yield(); break;
    case SYS_exit: sys_exit(a[1]); break;
    case SYS_write: sys_write(a[1], a[2], a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

/* pa3.2
 * 2020-12-11
 */
int sys_yield() {
  _yield();
  return 0;
}

void sys_exit(uintptr_t arg) {
  _halt(arg);
}

int sys_write(int fd, const void *buf, size_t len) {
  if (fd == 1 || fd == 2) {
    int i = 0;
    for ( ; i < len; i++) {
      _putc(((char *)buf)[i]);
    }
    return len;
  }
  return -1;
}