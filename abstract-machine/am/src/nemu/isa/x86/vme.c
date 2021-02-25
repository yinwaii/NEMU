#include <am.h>
#include <nemu.h>
#include <klib.h>

union LinearAddress
{
  struct {
    uintptr_t offset : 12;
    uintptr_t page : 10;
    uintptr_t dir : 10;
  };
  uintptr_t val;
};
union PageTableEntry
{
  struct {
    uintptr_t present : 1;
    uintptr_t read_write : 1;
    uintptr_t user_supervisor : 1;
    uintptr_t : 2;
    uintptr_t A : 1;
    uintptr_t dirty : 1;
    uintptr_t : 2;
    uintptr_t available : 3;
    uintptr_t address : 20;
  };
  uintptr_t val;
};

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    // printf("Mapping segment %d ...\n", i);
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  union LinearAddress la;
  la.val = (uintptr_t)va;
  union PageTableEntry *directory = (union PageTableEntry *)(as->ptr) + la.dir;
  if (directory->present == 0)
  {
    // printf("Add the pagesheet: %d in %p from page: %d\n", la.dir, directory, la.page);
    uintptr_t addr = (uintptr_t)pgalloc_usr(PGSIZE);
    directory->val = addr & 0xfffff000;
    directory->present = 1;
    // printf("%x\n", directory->val);
  }
  union PageTableEntry *page = (union PageTableEntry *)(directory->val & 0xfffff000) + la.page;
  page->val = (uintptr_t)pa & 0xfffff000;
  page->present = 1;
}

Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *context = (Context *)kstack.end - 1;
  memset(context, 0, sizeof(context));
  context->cs = 0x8;
  context->eip = (uintptr_t)entry;
  context->esp = (uintptr_t)kstack.end;
  context->irq = 0x81;
  context->GPRx = (uintptr_t)heap.end;
  return context;
}
