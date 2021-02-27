#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *old = pf;
  memset(pf, 0, nr_page * PGSIZE);
  pf += nr_page * PGSIZE;
  assert(pf < (void *)heap.end);
  // Log("new page allocated in %p", old);
  return old;
}

static inline void* pg_alloc(int n) {
  return new_page((n + PGSIZE - 1) / PGSIZE);
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  // Log("Before brk: %p", current->max_brk);
  if (brk > current->max_brk) {
    int page_num = (brk - current->max_brk + PGSIZE - 1) / PGSIZE;
    void *pg = new_page(page_num);
    for (int i = 0; i < page_num; i++)
      map(&current->as, (void *)(current->max_brk + i * PGSIZE), pg + i * PGSIZE, 0);
    current->max_brk += page_num * PGSIZE;
  }
  // Log("After brk");
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
