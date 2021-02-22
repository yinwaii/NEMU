#include <proc.h>
#include <loader.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%d' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void *)0x1);
  context_kload(&pcb[1], hello_fun, (void *)0x2);
  switch_boot_pcb();
  // naive_uload(NULL, "/bin/menu");

  Log("Initializing processes...");

  // load program here

}

Context* schedule(Context *prev) {
  static int current_pcb = 0;
  current->cp = prev;
  // current = &pcb[0];
  current = &pcb[current_pcb];
  current_pcb = 1 - current_pcb;
  return current->cp;
}
