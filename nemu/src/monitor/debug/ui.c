#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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

static int cmd_help(char *args);

/* pa1.1
 * 2020-11-03
 * implement command si, info, x
 */
static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  int count = 1;
  if (arg) {
    count = atoi(arg);
  }
  if (count <= 0){
    printf("%s : Parameter is invalid.\n", arg);
  }
  cpu_exec(count);
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  if (!arg) {
    printf("the commond 'info' miss parameters.\n");
  }
  else if (strcmp(arg, "r") == 0) {
    isa_reg_display();
  }
  else if (strcmp(arg, "w") == 0) {
    display_wp();
  }
  else {
    printf("Invalid parameter.\n");
  }
  return 0;
}

/* pa1.2
 * 2020-11-28
 * implements commond p
 * UPDATE: 2020-12-9
 * delete the first code 'char *args = strtok(NULL, " ");'
 * because do not need it(will make expr get a bad expression)
 *  */
static int cmd_p(char *args) {
  if (args == NULL) {
    printf("the commond 'p' miss parameters.\n");
  }
  else {
    bool success = true;
    uint32_t result = expr(args, &success);
    if (!success) {
      printf("Invalid expression '%s'\n", args);
    }
    else {
      printf ("0x%x(%u)\n", result, result);
    }
  }
  return 0;
}

//FIXME: EXPR here just is a immediate operend, but it should be expression
static int cmd_x(char *args) {
  char *arg1 = strtok(NULL, " ");
  char *arg2 = strtok(NULL, " ");
  if (arg1 == NULL || arg2 == NULL)
  {
    printf("the commomd 'x' miss parameters.\n");
  }
  else {
    int N = atoi(arg1);
    if (N <= 0)
    {
      printf("Invalid parameter.\n");
    }
    else {
      vaddr_t addr = 0x0;
      //FIXME: The validity of parameter arg2 is not determined,e.g:arg2 is "abfdefg0"
      //accroding sscanf to put in addr,then addr = abfedf,no warning and no error will be raise
      sscanf(arg2, "%x", &addr);
      int i;
      for (i = 0; i < N; i++, addr += 4) {
        uint32_t data = vaddr_read(addr, 4);
        printf("0x%08x%s", data, (i & 3) == 3?"\n" : "    ");
      }
      printf("\n");
    }
    
  }
  return 0;
}
/* pa1.3
 * 2020-11-28
 * implements commond w,d
 * */
static int cmd_w(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("the commond 'w' miss parameters.\n");
  }
  else {
    WP *p = new_wp();
    bool success = true;
    printf("val = %u\n", p->val);
    strcpy(p->expr, arg);
    p->val = expr(p->expr, &success);
    p->count = 0;
    if (!success) {
      printf("Expression: %s cannot caculate,setting watchlpoint failed.\n", p->expr);
      free_wp(p);
    }
    else {
      printf("Set watchpoint %d : %s\n", p->NO, p->expr);
    }
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("the commond d miss parameter.\n");
  }
  else {
    int NO = atoi(arg);
    if (NO < 0) {
      printf("Invalid parameter: %s.\n", arg);
    }
    else {
      delete_wp(NO);
    }
  }
  return 0;
}

/* pa3.3 
 * definded in nemu/src/monitor/diff-test/diff-test.c */
void difftest_detach();
void difftest_attach();

static int cmd_detach(char * args) {
  difftest_detach();
  return 0;
}

static int cmd_attach(char *args) {
  difftest_attach();
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* pa1.1 add more commands */
  { "si", "'si [N]' Single step N times,when N is not given,the default is 1", cmd_si },
  { "info", "'info r' Print register state;'info w' Print watchpoint informations", cmd_info },
  { "p", "'p EXPR' Evaluate the EXPR", cmd_p },
  { "x", "'x N EXPR' Evaluate the EXPR,using the result as staring address which outputs N consecutive 4 bytes in hex", cmd_x },
  { "w", "'w EXPR' When the value of EXPR changed,pause the execution of the program", cmd_w },
  { "d", "'d N' Delete watchpoint numbered N", cmd_d },
  /* pa3.3 */
  { "detach", "'detach' Quit Diff-Test mode", cmd_detach },
  { "attach", "'attach' Open Diff-Test mode", cmd_attach }
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
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
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
