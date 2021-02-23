#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char cmd_list[30][30];

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char *argv_list[30];
  setenv("PATH", "/bin", 0);
  int cmd_cnt = 0;
  while (*cmd && *cmd != '\n')
  {
    while(*cmd && *cmd == ' ')
      cmd++;
    sscanf(cmd, "%s", cmd_list[cmd_cnt]);
    // printf("%s\n", cmd_list[cmd_cnt]);
    argv_list[cmd_cnt] = cmd_list[cmd_cnt];
    cmd_cnt++;
    while(*cmd && *cmd != ' ' && *cmd != '\n')
      cmd++;
    // printf("@%c@\n", *cmd);
  }
  argv_list[cmd_cnt] = NULL;
  // for (int i = 0; i <= cmd_cnt; i++)
    // printf("%s ", argv_list[i]);
  // printf("\n");
  execvp(argv_list[0], argv_list);
  // printf("%s %s %d\n", cmd_list[0], argv_list[0], cmd_cnt);
  // sscanf(cmd, "%s", name);
  // printf("@%s@\n", name);
  // char hello_args[20][20] = {"/bin/exec-test", "666"};
  // char *list[10] = {hello_args[0], hello_args[1]};
  // execvp("exec-test", list);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
