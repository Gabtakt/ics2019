#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  /* pa3.2
   * 2020-12-11
   * FIXME: in this step, parameter 'pcb' and 'filename' can be ignored
   */
  /* pa3.3
   * 2020-12-13
   * FIX: use 'filename' in this step
   */
  Elf_Ehdr elf_header;
  int fd = fs_open(filename, 0, 0);
  assert(fd != -1);
  // read the elf header file from start
  fs_read(fd, &elf_header, sizeof(Elf_Ehdr));
  size_t open_offset = fs_lseek(fd, elf_header.e_phoff, SEEK_SET);

  uint16_t e_phnum = elf_header.e_phnum;
  Elf_Phdr program_header;
  // read all the program header file
  while (e_phnum--) {
    open_offset += fs_read(fd, &program_header, sizeof(Elf_Phdr));
    assert(fd != -1);
    /* the segment should be loaded.
     * Mem[p_vaddr + p_filesz - 1, p_vaddr] <- Mem[p_offset + p_filesz - 1, p_offset]
     * Mem[p_vaddr + p_filesz + p_memsz - 1, p_vaddr + p_filesz] <- 0
     */
    if (program_header.p_type == PT_LOAD) {
      fs_lseek(fd, program_header.p_offset, SEEK_SET);
      fs_read(fd, (void *)program_header.p_vaddr, program_header.p_filesz);
      memset((void *)program_header.p_vaddr + program_header.p_filesz, 0, program_header.p_memsz - program_header.p_filesz);
    }
    fs_lseek(fd, open_offset, SEEK_SET);
  }
  fs_close(fd);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
