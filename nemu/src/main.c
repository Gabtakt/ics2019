#include "monitor/expr.h"
int init_monitor(int, char *[]);
void ui_mainloop(int);

void test_expr() {
	FILE *fp = fopen("tools/gen-expr/input", "r");
	if(fp == NULL) {
		printf("File: %s open failed\n", "tools/gen-expr/input");
		return ;
	}
	Log("Testing the expr eval...\n");

	uint32_t std_ret, my_ret;
	bool success;
	char Expr[6000];
	bool pass = true;
	int count = 0, passed = 0;
	while(fscanf(fp, "%u %s", &std_ret, Expr) != EOF) {
		count++;
		success = true;
		my_ret = expr(Expr, &success);
		if(!success || my_ret != std_ret){
			pass = false;
			printf("%s\n",Expr);
			// printf("expr test failed at[%d]: %s\n", count, Expr);
			// printf("std result: %u, my result %u\n", std_ret, my_ret);
		}else{
			// printf("test passed at: %s\n", Expr);
			// printf("std result: %u, my result %u\n", std_ret, my_ret);
			passed++;
		}
	}
	//printf("total passed: [%d/%d].\n",passed, count);
}	


int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  test_expr();

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
