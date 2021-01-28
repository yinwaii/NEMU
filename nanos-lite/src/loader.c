#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0x0, sizeof(Elf_Ehdr));
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    Elf_Phdr phdr;
    ramdisk_read(&phdr, ehdr.e_phoff + ehdr.e_phentsize * i, ehdr.e_phentsize);
    if (phdr.p_type == PT_LOAD) 
    {
      memset((void *)phdr.p_vaddr, 0, phdr.p_memsz);
      ramdisk_read((void *)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
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

