#include "common.h"
#include "syscall.h"
#include "proc.h"

/* all syscall funciton declear here*/
void sys_exit(uintptr_t arg);
int sys_yield();
int sys_open(const char *pathname, int flags, int mode);
int sys_read(int fd, void *buf, size_t len);
int sys_write(int fd, const void *buf, size_t len);
int sys_close(int fd);
int sys_lseek(int fd, size_t offset, int whence);
int sys_brk(uintptr_t increment);
int sys_execve(const char *fname, char * const argv[], char *const envp[]);

/* all file system function(used by syscall) */
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

extern void naive_uload(PCB *pcb, const char *filename);

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
    case SYS_exit: sys_exit(a[1]); break;
    case SYS_yield: c->GPRx = sys_yield(); break;
    case SYS_open: c->GPRx = sys_open((char*)a[1],a[2],a[3]); break;
    case SYS_read: c->GPRx = sys_read(a[1],(void*)a[2],a[3]); break;
    case SYS_write: c->GPRx = sys_write(a[1], (void *)a[2], a[3]); break;
    case SYS_close: c->GPRx = sys_close(a[1]); break;
    case SYS_lseek: c->GPRx = sys_lseek(a[1],a[2],a[3]); break;
    case SYS_brk: c->GPRx = sys_brk(a[1]); break;
    case SYS_execve: c->GPRx = sys_execve(a[1], a[2], a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

/* pa3.2
 * 2020-12-11
 */
void sys_exit(uintptr_t arg) {
  //_halt(arg);
  /* pa3.3
   * 2020-12-15
   * call sys_execve
   */
  sys_execve("/bin/init", NULL, NULL);
}

int sys_yield() {
  _yield();
  return 0;
}

int sys_open(const char *pathname, int flags, int mode) {
  return fs_open(pathname, flags, mode);
}

int sys_read(int fd, void *buf, size_t len) {
  return fs_read(fd, buf, len);
}

int sys_write(int fd, const void *buf, size_t len) {
  return fs_write(fd, buf, len);
}

int sys_close(int fd) {
  return fs_close(fd);
}

int sys_lseek(int fd, size_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

int sys_brk(uintptr_t increment){ 
  return 0;
}

int sys_execve(const char *fname, char * const argv[], char *const envp[]) {
  // FIXME: ignord parameter argv and envp, use them later
  naive_uload(NULL, fname);
}