#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>


// this should be enough
static char buf[65536] = {};
static unsigned buf_pt = 0;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static inline unsigned choose(unsigned n) {
  return rand() % n;
}

static inline void gen(char c) {
  char add_str[] = {c, '\0'};
  assert(strlen(buf) + strlen(add_str) < 30000);
  strcat(buf, add_str);
}

static inline void gen_num() {
  char num_str[35];
  sprintf(num_str,"%u",choose((unsigned)(-1)));
  // strcat(num_str, "u");
  assert(strlen(buf) + strlen(num_str) < 30000);
  strcat(buf, num_str);
}

static inline void gen_rand_op() {
  char op_list[] = {'+', '-', '*','/'};
  char add_str[] = {op_list[choose(4)], '\0'};
  assert(strlen(buf) + strlen(add_str) < 30000);
  strcat(buf, add_str);
}

static inline void gen_space() {
  for(int i=0; i < choose(5); i++) {
	if(choose(100) > 90)
	{
	  assert(strlen(buf) + 1 < 30000);
	  strcat(buf," ");
	}
  }
}

static inline void gen_rand_expr() {
  gen_space();
  switch (choose(4)) {
	case 0: case 3:  gen_num(); break;
	// case 2: gen('-'); gen_num(); break;
	// case 3: gen('-'); gen('('); gen_rand_expr(); gen(')'); break;
	case 1: gen('('); gen_rand_expr(); gen(')'); break;
	default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
  gen_space();
  // printf("%s", buf);
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	buf[0]='\0';
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr -w");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%u", &result);
    pclose(fp);

	printf("%u %s\n", result, buf);
  }
  return 0;
}
