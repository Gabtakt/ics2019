#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define NR_TOKEN 10
#define BUF_LEN 40960
// this should be enough
static char buf[BUF_LEN];
static char ops[] = "+-*/";
int nr_token = 0, len = 0;

static uint32_t choose(uint32_t n) {
  return rand() % n;
}

static inline void gen_rand_expr(int l,int r) {
  int pos;
  if (l == r){
    buf[l] = choose(9)+'1';
  }
  else if (l == r - 1) {
	  buf[l] = choose(9)+'1';
	  buf[r] = choose(9)+'1';
  }
  else {
	  switch(choose(1)) {
		  case 0:
        pos = l + 1 + choose(r - l - 1);
			  gen_rand_expr(l, pos - 1);
			  buf[pos] = ops[choose(4)];
			  gen_rand_expr(pos + 1, r);
			  break;
		  default:
			  buf[l]='(';
			  buf[r]=')';
			  gen_rand_expr(l + 1, r - 1);
			  break;
	  }
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr(0, NR_TOKEN);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
