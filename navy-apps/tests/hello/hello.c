#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      for (int k = 0; k < argc; k++)
        printf("argv: %s\n", argv[k]);
      for (int k = 0; k < argc; k++)
        printf("envp: %s\n", envp[k]);
      printf("Hello World from Navy-apps for the %dth time!\n", i++);
      j = 0;
    }
  }
  return 0;
}
