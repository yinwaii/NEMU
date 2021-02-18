#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <monitor/difftest.h>

void cpu_exec(uint64_t);
word_t expr(char *,bool *);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  int si_N = (args == NULL) ? 0 : atoi(args);
  if (si_N <= 0)
	cpu_exec(1);
  else
	cpu_exec(si_N);
  return 0;
}

static int cmd_info(char *args) {
  if (args != NULL)
  {
	char *subcmd = strtok(args, " ");
	if (strcmp(subcmd, "r") == 0)
	{
	  isa_reg_display();
	}
	else if(strcmp(subcmd, "w") == 0)
	{
	  if(get_wp_head() == NULL) {
		printf("There is no watchpoint yet!\n");
		return 0;
	  }
	  printf("%-5s%-20s%-20s%-20s\n","ID", "Expression", "Value(dec)", "Value(hex)");
	  for (WP *tmp = get_wp_head(); tmp != NULL; tmp = tmp->next)
		printf("%-5d%-20s%-20d%#-20x\n", tmp->NO, tmp->expr, tmp->tmp_value, tmp->tmp_value);
	}
  }
  return 0;
}

static int cmd_p(char *args) {
  if (args != NULL)
  {
	bool *success = (bool *)malloc(1);
	if(strcmp(args, "test") == 0) {
	  char file[3][30] = {"build/", "build/o", "build/n"};
	  printf("Please input the file name (whose length should <= 10) of test data:\n");
	  char base[11];
    int num = 0;
    num = scanf("%s", base);
    assert(num >= 0);
    unsigned answer;
    char e[66666];
	  FILE *fp = fopen(strcat(file[0], base), "r");
	  FILE *flog = fopen(strcat(file[1], base), "w");
	  FILE *fnp = fopen(strcat(file[2], base), "w");
	  assert(fp != NULL);
	  assert(flog != NULL);
	  assert(fnp != NULL);
	  int i = 0;
	  while(fscanf(fp,"%u %[^\n]", &answer, e) > 0) {
		i++;
		unsigned result = expr(e, success);
		if(result != answer) {
		  fprintf(flog, "list: %d\nresult: %u answer: %u\nexpression:%s\n", i, result, answer, e);
		  fprintf(fnp, "%u %s\n", answer, e);
		}
		// else
		//   fprintf(flog, "list: %d checked\n", i);
	  }
	  fclose(fp);
	  fclose(flog);
	  fclose(fnp);
	  printf("Test finished.\n");
	}
	else {
	  unsigned result = expr(args, success);
	  printf("dec: %d\nhex: %#x\n", result, result);
	}
	free(success);
  }
  return 0;
}

static int cmd_x(char *args)
{
  int N;
  bool *success = (bool *)malloc(1);
  strtok(args, " ");
  N = atoi(args);
  args += strlen(args) + 1;
  unsigned result = expr(args, success);
  printf("%-10s%-15s%-10s\n", "Address", "Value(dec)", "Value(hex)");
  unsigned value = paddr_read(result + N, 4);
  for (int i = 0; i < N; i++) {
	printf("%-10x%-15d%#-10x\n", result + i, value, value);
  }
  return 0;
}

static int cmd_w(char *args)
{
  WP* tmp = new_wp();
  strcpy(tmp->expr, args);
  bool *success = (bool *)malloc(1);
  tmp->tmp_value = expr(args, success);
  free(success);
  return 0;
}

static int cmd_d(char *args)
{
  int N = 0;
  sscanf(args, "%d", &N);
  WP *tmp = get_wp_head();
  assert(tmp != NULL);
  for(; tmp != NULL && tmp->NO != N; tmp = tmp->next)
	;
  assert(tmp != NULL);
  free_wp(tmp);
  return 0;
}

static int cmd_help(char *args);

#ifdef DIFF_TEST
static int cmd_detach(char *args) 
{
  difftest_detach();
  return 0;
}

static int cmd_attach(char *args)
{
  difftest_attach();
  return 0;
}
#endif
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Let the program step through N instructions and then pause execution, and the default N is 1", cmd_si },
  { "info", "Print the status of registers (info r) or monitoring points (info w)", cmd_info },
  { "p", "Print the value of a expression", cmd_p },
  { "x", "Get the value of a expression, use the result as the starting memory address, and output N consecutive 4 bytes in hexadecimical form", cmd_x },
  { "w", "Set the watchpoint, when the values of watchpoints are changed, the system will pause", cmd_w },
  { "d", "Delete the watchpoint whose id is N", cmd_d }
  #ifdef DIFF_TEST
  ,{ "detach", "Close the DiffTest mode", cmd_detach },
  { "attach", "Open the Difftest mode", cmd_attach }
  #endif
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) { 
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else { 
    for (i = 0;  i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the r emaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
