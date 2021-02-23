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

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  // Log("%p", envp);
  // for (int i = 0; envp[i] != NULL; i++)
  //   Log("1%d: %s", i, envp[i]);

  // for (int i = 0; envp[i] != NULL; i++)
  //   Log("2%d: %s", i, envp[i]);
  uintptr_t ustack = (uintptr_t)(new_page(8) + 8 * PGSIZE);
  // for (int i = 0; envp[i] != NULL; i++)
  //   Log("3%d: %s", i, envp[i]);
  uintptr_t num_argv = 0;
  uintptr_t num_envp = 0;
  if (argv == NULL)
    num_argv = 0;
  else
  {
    while (argv[num_argv] != NULL)
      num_argv++;
  }
  // printf("%p %p\n", envp, envp[0]);
  if (envp == NULL)
    num_envp = 0;
  else
  {
    while (envp[num_envp] != NULL)
      num_envp++;
  }
  uintptr_t *map_argv = malloc((num_argv + 1) * sizeof(uintptr_t));
  uintptr_t *map_envp = malloc((num_envp + 1) * sizeof(uintptr_t));
  // printf("%d %d\n", num_argv, num_envp);
  for (int i = 0; i < num_argv; i++)
  {
    // printf("%d %s\n", i, argv[i]);
    ustack -= strlen(argv[i]) + 1;
    // printf("%d\n", i);
    strcpy((char *)(ustack), argv[i]);
    map_argv[i] = ustack;
  }
  for (int i = 0; i < num_envp; i++)
  {
    // printf("%d %s\n", i, envp[i]);
    ustack -= strlen(envp[i]) + 1;
    strcpy((char *)(ustack), envp[i]);
    map_envp[i] = ustack;
  }
  // printf("bbb\n");
  map_argv[num_argv] = (uintptr_t)NULL;
  map_envp[num_envp] = (uintptr_t)NULL;
  ustack -= (num_envp + 1) * sizeof(uintptr_t);
  memcpy((void *)(ustack), (void *)map_envp, (num_envp + 1) * sizeof(uintptr_t));
  ustack -= (num_argv + 1) * sizeof(uintptr_t);
  memcpy((void *)(ustack), (void *)map_argv, (num_argv + 1) * sizeof(uintptr_t));
  ustack -= sizeof(uintptr_t);
  memcpy((void *)(ustack), (void *)&num_argv, sizeof(uintptr_t));
  // printf("aaa\n");

  uintptr_t entry = loader(pcb, filename);
  Area kstack = {(void *)pcb->stack, (void *)(pcb->stack + 1)};
  pcb->cp = ucontext(NULL, kstack, (void *)entry);
  pcb->cp->GPRx = ustack;
}