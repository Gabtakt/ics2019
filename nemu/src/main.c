#include "monitor/expr.h"

//#define EXPR_TEST

int init_monitor(int, char *[]);
void ui_mainloop(int);

#ifdef EXPR_TEST
void test_expr() {
	FILE *fp = fopen("tools/gen-expr/input", "r");
	if(fp == NULL) {
		printf("File: %s open failed\n", "tools/gen-expr/input");
		return ;
	}
	uint32_t std_ret, expr_ret;
	bool success;
	char exp[6000];
	int count = 0, passed = 0;
	while(fscanf(fp, "%u %s", &std_ret, exp) != EOF) {
		count++;
		success = true;
		expr_ret = expr(exp, &success);
		if(!success || expr_ret != std_ret) {
			printf("expr test failed at[%d]: %s\n", count, exp);
			printf("std result: %u, my result %u\n", std_ret, expr_ret);
		}else{
			passed++;
		}
	}
	printf("total passed: [%d/%d].\n",passed, count);
}	
#endif

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* test pa1.2 */
  #ifdef EXPR_TEST
  test_expr();
  #endif

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
