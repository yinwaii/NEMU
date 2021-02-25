#include <memory.h>

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
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
