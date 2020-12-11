#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  /* pa3.2
   * 2020-12-11
   * in this step, parameter 'pcb' and 'filename' can be ignored
   * ELF
   * 
   */
  // Elf_Ehdr elf_header;
  // // read the elf header file from start
  // ramdisk_read(&elf_header, 0x0, sizeof(Elf_Ehdr));
  // Elf32_Off phoff = elf_header.e_phoff;
  // uint32_t e_phnum = elf_header.e_phnum;
  // Elf_Phdr program_header;
  // while (e_phnum--) {
  //   // read all the program header file
  //   ramdisk_read(&program_header, phoff, sizeof(Elf_Phdr));
  //   /* the segment should be loaded,
  //    * Mem[p_vaddr + p_filesz - 1, p_vaddr] <- Mem[p_offset + p_filesz - 1, p_offset]
  //    * Mem[p_vaddr + p_filesz + p_memsz - 1, p_vaddr + p_filesz] <- 0
  //    */
  //   if (program_header.p_type == PT_LOAD) {
  //     uint8_t buf[program_header.p_filesz];
  //     ramdisk_read(&buf, program_header.p_offset, program_header.p_filesz);
  //     memcpy((void *)program_header.p_vaddr, &buf, program_header.p_filesz);
  //     memset((void *)program_header.p_vaddr + program_header.p_filesz, 0, program_header.p_memsz);
  //   }
  //   phoff += sizeof(Elf_Phdr);
  // }
  // return elf_header.e_entry;

  Elf_Ehdr elfheader;
  Elf_Phdr programheader;
  size_t offset = 0;
  size_t p_offset = 0;
  size_t len = (size_t)sizeof(Elf_Ehdr);
  
  ramdisk_read(&elfheader,offset,len);
  offset = elfheader.e_phoff;
  for (uint16_t i=0; i<elfheader.e_phnum; i++){

    ramdisk_read(&programheader,offset,(size_t)sizeof(Elf_Phdr));
    offset+=sizeof(Elf_Phdr);
    if(programheader.p_type == PT_LOAD){
      uint8_t buf[programheader.p_filesz];
      ramdisk_read(&buf,programheader.p_offset,programheader.p_filesz);
      memcpy((void*)programheader.p_vaddr,&buf,programheader.p_filesz);
      memset((void*)(programheader.p_vaddr+programheader.p_filesz),0,(programheader.p_memsz-programheader.p_filesz));
    }

  }
  return elfheader.e_entry;
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
