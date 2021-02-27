#include <proc.h>
#include <loader.h>

int fg_pcb = 1;
PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
PCB pcb_boot = {};
PCB *current = NULL;

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
  char hello_args[20][20] = {"/bin/pal", "666"};
  char *list[10] = {hello_args[0], hello_args[1]};
  // printf("%s %s\n", list[0], list[1]);
  context_uload(&pcb[0], "/bin/hello", list, list);
  context_uload(&pcb[1], "/bin/menu", list, list);
  context_uload(&pcb[2], "/bin/bird", list, list);
  context_uload(&pcb[3], "/bin/pal", list, list);
  // context_kload(&pcb[0], hello_fun, (void *)0x2);
  // context_kload(&pcb[1], hello_fun, (void *)0x1);
  // context_uload(&pcb[1], "/bin/pal");
  switch_boot_pcb();
  // naive_uload(NULL, "/bin/menu");

  Log("Initializing processes...");

  // load program here

}

Context* schedule(Context *prev) {
  static int count = 0;
  current->cp = prev;
  // current = &pcb[0];
  if (count % 100 == 0)
    current = &pcb[0];
  else
    current = &pcb[fg_pcb];
  count++;
  // Log("After scheduling");
  return current->cp;
}
