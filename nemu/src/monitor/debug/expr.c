#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  /* TODO: Add more token types */
  TK_NUM,
  TK_HEX,
  TK_REG,
  TK_PLUS,
  TK_MINUS,
  TK_MUL,
  TK_DIV,
  TK_EQ,
  TK_AND,
  TK_OR,
  TK_LBR,
  TK_RBR
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},                 // spaces
  {"0[Xx][0-9a-fA-f]+", TK_HEX},     // hex
  {"[0-9]+", TK_NUM},                // num
  {"\\$[a-zA-z]{2,3}", TK_REG},      // register
  {"\\+", TK_PLUS},                  // plus
  {"-", TK_MINUS},                   // minus
  {"\\*", TK_MUL},                   // mutiply
  {"/", TK_DIV},                     // divide
  {"==", TK_EQ},                     // equal
  {"&&", TK_AND},                    // logical and
  {"\\|\\|", TK_OR},                 // logical or
  {"\\(", TK_LBR},                   // left bracket
  {"\\)", TK_RBR},                   // right bracket
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (substr_len >= 32)
        {
          Log("Token is too long.\n");
          assert(0);
          return false;
        }
        if (nr_token > 32)
        {
          Log("Parameter 'nr_token' is out of range.\n");
          assert(0);
          return false;
        }
        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_NUM:
          case TK_HEX:
          case TK_REG:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[nr_token] = '\0';
          // public action
          default:
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
