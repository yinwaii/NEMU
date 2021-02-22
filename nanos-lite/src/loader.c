#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  if (filename == NULL)
    panic("No filename given!");
  Elf_Ehdr ehdr;
  int fd = fs_open(filename, 0, 0);
  fs_lseek(fd, 0x0, SEEK_SET);
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  // ramdisk_read(&ehdr, 0x0, sizeof(Elf_Ehdr));
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    Elf_Phdr phdr;
    fs_lseek(fd, ehdr.e_phoff + ehdr.e_phentsize * i, 0);
    fs_read(fd, &phdr, ehdr.e_phentsize);
    // ramdisk_read(&phdr, ehdr.e_phoff + ehdr.e_phentsize * i, ehdr.e_phentsize);
    if (phdr.p_type == PT_LOAD) 
    {
      memset((void *)phdr.p_vaddr, 0, phdr.p_memsz);
      fs_lseek(fd, phdr.p_offset, 0);
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      // ramdisk_read((void *)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
    }
  }
  // TODO();
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry, void *arg) {
  Area kstack = {(void *)pcb->stack, (void *)(pcb->stack + 1)};
  pcb->cp = kcontext(kstack, entry, arg);
  // Context *c = pcb->cp;
  // printf("%p, %p, %p, %p, %p, %p, %p, %p\n", c->edi, c->esi, c->ebp, c->esp, c->ebx, c->edx, c->ecx, c->eax);
  // printf("%p\n", c->irq);
  // printf("%p, %p, %p\n", c->eip, c->cs, c->eflags);
  // printf("%p\n", *(c->esp + 8));
}