#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uintptr_t argc = *args;
  args++;
  char **argv = (char **)args;
  args += argc;
  args++;
  char **envp = (char **)args;
  // char *empty[] = {NULL};
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
