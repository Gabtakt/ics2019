#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

/* pa1.2
 * 2020-11-26
 * implements calculate the value of the expression
 * */
enum {
  TK_NOTYPE = 256,
  TK_NUM,
  TK_HEX,
  TK_REG,
  TK_PLUS,
  TK_MINUS,
  TK_MUL,
  TK_DIV,
  TK_NEG,
  TK_DEREF,
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

uint32_t isa_reg_str2val(const char *s,bool *success);
int check_parentheses(int p, int q);
uint32_t expr(char *e, bool *success);
int get_main_op(int p, int q);
uint32_t eval(int p, int q, bool *success);

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

static Token tokens[65536] __attribute__((used)) = {};
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

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* the string length is out of range */
        if (substr_len >= 32) {
          Log("Token is too long.\n");
          assert(0);
          return false;
        }
        /* the number of tokens is out of range */
        if (nr_token >= 65536) {
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
            // strncpy(tokens[nr_token].str, substr_start, substr_len);
            // tokens[nr_token].str[nr_token] = '\0';
          // public action
          default:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[nr_token] = '\0';
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
  int i;
  // find dereferance op and fix its type
  for (i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_MUL && (i == 0 || (tokens[i - 1].type != TK_NUM &&
        tokens[i - 1].type != TK_HEX && tokens[i - 1].type != TK_REG && tokens[i - 1].type != TK_RBR))) {
      tokens[i].type = TK_DEREF;
    }
    else if (tokens[i].type == TK_MINUS && (i == 0 || (tokens[i - 1].type != TK_NUM &&
             tokens[i - 1].type != TK_HEX && tokens[i - 1].type != TK_REG && tokens[i - 1].type != TK_RBR))) {
      tokens[i].type = TK_NEG;
    }
  }
  return eval(0, nr_token - 1, success);
}

/* case 1: return  1 if it is a valid expression surrounded by a pair of parentheses.
 * case 2: return  0 if it is a valid expression but not surrounded by a pair of parentheses.
 * case 3: return -1 if it is a bad expression.
 */
int check_parentheses(int p, int q) {
  bool flag = (tokens[p].type == TK_LBR && tokens[q].type ==TK_RBR);
	int count = 0, i;
  // match parentheses
	for(i = p; i <= q; i++) {
		if(tokens[i].type == TK_LBR) {
      count++;
    }
		else if(tokens[i].type == TK_RBR) {
      count--;
    }
    /* case 3: ')' more than '(', it is a bad expression */
		if(count < 0) {
      return -1;
    }
	}
  /* case 3: the number of '(' and ')' is not equal */
	if(count != 0) {
    return -1;
  }
  /* case 2 */
	if(!flag) {
    return 0;
  }

	/* special case 2, eg: '(1 + 2) * (3 + 4)' should return 0 rather than 1.
   * before here we matched the parentheses and makesure the position of
   * '(' is p and the position of ')' is q, then if we find any ')' or
   * '(' in the range of p+1 ~ q-1, we can assert it is a special case. 
   */
	for(i = p + 1; i <= q - 1; i++) {
		if(tokens[i].type == TK_LBR || tokens[i].type == TK_RBR) {
      return 0;
    }
	}
	return 1;
}

/* return the priority of the op */
int get_priority(int type) {
	int priority = 0;
	switch(type) {
    case TK_NOTYPE:
    case TK_NUM:
    case TK_HEX:
    case TK_REG:
      priority = 0;
      break;
		case TK_OR:
			priority = 1;
			break;
		case TK_AND:
			priority = 2;
			break;
		case TK_EQ:
			priority = 3;
			break;
		case TK_PLUS:
		case TK_MINUS:
			priority = 4;
			break;
		case TK_MUL:
		case TK_DIV:
			priority = 5;
			break;
		case TK_DEREF:
    case TK_NEG:
			priority = 6;
			break;
		default:
			priority = 7;
	}
	return priority;
}

/* return the position of the main op, if there is no main op, return -1 */
int get_main_op(int p, int q) {
	int inBracket = 0, i, pos = -1;
  int MIN_PRIORITY = get_priority(TK_REG), MAX_PRIORITY = get_priority(TK_LBR);
  int cur_min_priority = MAX_PRIORITY,cur_priority = cur_min_priority;
	for(i = p; i <= q; i++) {
		int type = tokens[i].type;
    cur_priority = get_priority(type);
    /* if it in the parentheses and op is not '(' or ')' or number,it is op */
		if(!inBracket && cur_priority > MIN_PRIORITY && cur_priority < MAX_PRIORITY) {
      /* get the position of the most right mian op  */
			if (cur_priority <= cur_min_priority) {
        pos = i;
        cur_min_priority = cur_priority;
      }
		}
		else if(type == TK_LBR) {
      inBracket++;
    }
		else if(type == TK_RBR) {
      inBracket--;
    }
	}
	return pos;
}

/*add a parameter to judge whether the eval is success. */
uint32_t eval(int p, int q, bool *success)
{
  /* bad expression */
  if (p > q) {
    *success = false;
    return 0;
  }
  /* single token, it is a number or register */
  else if (p == q) {
    int type = tokens[p].type;
    if (type == TK_NUM || type == TK_HEX) {
      return strtoul(tokens[p].str, NULL, 0);
    }
    if (type == TK_REG) {
      uint32_t val = isa_reg_str2val(tokens[p].str + 1, success);
      if (!(*success)) {
        printf("Unkown register: %s.\n", tokens[p].str);
      }
      return val;
    }
    /* bad expression */
    *success = false;
    return 0;
  }
  int ret = check_parentheses(p, q);
  if (ret == -1) {
    *success = false;
    printf("Parentheses not match.\n");
    return 0;
  }
  if (ret == 1) {
    return eval(p + 1, q - 1, success);
  }
  /* check_parentheses returned 0 */
  int pos = get_main_op(p, q);
  if(pos == -1) {
    *success = false;
    return 0;
  }
  uint32_t left_val = 0, right_val = 0, val = 0;
  if (tokens[pos].type != TK_DEREF && tokens[pos].type != TK_NEG) {
    left_val = eval(p, pos - 1, success);
  }
  if (!(*success)) {
    return 0;
  }
  right_val = eval(pos + 1, q, success);
  if (!(*success)) {
    return 0;
  }
  switch (tokens[pos].type) {
    case TK_PLUS:
      val =  left_val + right_val;
      break;
    case TK_MINUS:
      val = left_val - right_val;
      break;
    case TK_MUL:
      val = left_val + right_val;
      break;
    case TK_DIV:
      if (right_val == 0) {
        printf("The dividend cannot be zero.\n");
        *success = false;
        return 0;
      }
      val = left_val / right_val;
      break;
    case TK_OR:
      val = left_val || right_val;
      break;
    case TK_AND:
      val = left_val && right_val;
      break;
    case TK_EQ:
      val = (left_val == right_val);
      break;
    case TK_DEREF:
      val = vaddr_read(right_val, 4);
    case TK_NEG:
      val = -right_val;
      break;
    default:
      printf("Unknown token %s.\n",tokens[pos].str);
      break;
  }
  return val;
}

