#include <isa.h>
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_BRACKET_LEFT, TK_BRACKET_RIGHT

  /* TODO: Add more token types */

};

static int token_priority(int TOKEN) {
  switch (TOKEN) {
	case TK_BRACKET_LEFT: case TK_BRACKET_RIGHT: return 10; break;
	case '*': case '/': return 2; break;
	case '+': case '-': return 1; break;
	case TK_EQ: return 0; break;
	default: return -1;
  }
}

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},			// spaces
  {"\\(", TK_BRACKET_LEFT},		// left-bracket
  {"\\)", TK_BRACKET_RIGHT},	// right-bracket
  {"[0-9]+", TK_NUM},			// numbers
  {"\\*", '*'},					// multiply
  {"/", '/'},					// division
  {"\\+", '+'},					// plus
  {"-", '-'},					// minus
  {"==", TK_EQ},			    // equal
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

  for (i = 0; i < NR_REGEX; i++) {
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

        switch (rules[i].token_type) {

		  case TK_BRACKET_LEFT: case TK_BRACKET_RIGHT: case '+': case '-': case '*': case '/': case TK_EQ: 
			// Push the "real token" into tokens
			if (nr_token + 1 >= 32)
			  panic("The expression is too long!");
			tokens[nr_token].type = rules[i].token_type;
			strcpy(tokens[nr_token].str, "");
			nr_token++;
			break;

		  case TK_NUM:
			// When the number can't be put into int
			if(substr_len > 31)
			  panic("The number is too big!");

			// the codes for high-accuracy evaluation
			/* while(substr_len > 31)
			 * {
			 *   if(nr_token + 1 >= 32)
			 *     panic("The expression is too long!");
			 *   tokens[nr_token].type = TK_NUM;
			 *   strncpy(tokens[nr_token].str, substr_start, 31);
			 *   tokens[nr_token].str[31] = '\0';
			 *   nr_token++;
			 *   substr_len -= 31;
			 *   substr_start += 31;
			 *   }
			 */

			// Push the numbers into tokens
			if (nr_token + 1 >= 32)
			  panic("The expression is too long!");
			tokens[nr_token].type = TK_NUM;
			// The substring doesn't have a '\0'
			strncpy(tokens[nr_token].str, substr_start, substr_len);
			tokens[nr_token].str[substr_len] = '\0';
			nr_token++;
			break;
	   // default: TODO();
        }
		
        break;
      }

    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  /* Test the tokens[] and nr_token */
  /* for(int i = 0; i < nr_token; i++)
   *   printf("%-5d%-10s\n", tokens[i].type, tokens[i].str);
   */

  return true;
}

static bool check_parentheses(int p, int q) {
  // debug:
  printf("p:%d q:%d\n",p,q);

  // It can't be ().
  if (q <= p + 1)
	return false;
	// panic("Empty bracket!");
  // It must be surrounded by a pair of parentheses.
  if (tokens[p].type != TK_BRACKET_LEFT || tokens[q].type != TK_BRACKET_RIGHT)
	return false;
  // Check whether it is matched with a stack (parentheses_number).
  int parentheses_number = 0;
  for (int i = p + 1; i <= q - 1; i++) {
	// debug:
	// printf("%d %c", parentheses_number, tokens[i].type);
	if (tokens[i].type == TK_BRACKET_LEFT)
	  parentheses_number++;
	else if(tokens[i].type ==TK_BRACKET_RIGHT) {
	  // Deal with the ")(".
	  if (parentheses_number <= 0)
		// panic("The expression has extra rights brackets at %d!", i);
		return false;
	  else
		parentheses_number--;
	}
	// debug:
	// printf(" %d\n", parentheses_number);
  }
  // Check the lost '('.
  if (parentheses_number == 0)
	return true;
  else {
	if (parentheses_number < 0)
	  panic("The expression has extra right brackets!");
	else
	  panic("The expression has extra left brackets!");
	// return false;
  }
}

static word_t eval (int p, int q) {
  if (p < 0 || q < 0 || p > q) 
	panic("Bad expression!");
  else if (p == q) {
	/* Single token.
	 * And the token must be a number.
	 */
	return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
	/* The expression is surrounded by a matched pair of parentheses.
	 * So we just throw away the parentheses.
	 */
	return eval(p + 1, q - 1);
  }
  else {
	int main_op = -1;
	// Find the main operator.
	int parentheses_number = 0;
	for (int i = p; i <= q; i++) {
	  switch(tokens[i].type) {
	    // Numbers are not main operators.
		case TK_NUM: continue; break;
	    // Tokens in brackets are not main operators.
		case TK_BRACKET_LEFT: parentheses_number++; break;
		case TK_BRACKET_RIGHT: parentheses_number--; break;
		default:
		  // If the token has no more priority, swap it with main op)
		  if(parentheses_number == 0 && (main_op == -1 || token_priority(tokens[i].type) <= token_priority(tokens[main_op].type)))
		  {
			// debug:
			// printf("%c:%d %c:%d\n",tokens[i].type,token_priority(tokens[i].type),tokens[main_op].type,token_priority(tokens[main_op].type));
			main_op = i;
		  }
	  }
	}
	bool single_operands = (p == main_op);
	word_t val1 = 0;
	if(single_operands == false) {
	  printf("double operands!");
	  val1 = eval(p, main_op - 1);
	}
	word_t val2 = eval(main_op + 1, q);
	// debug
	// printf("%d %c %d\n",val1,tokens[main_op].type,val2);

	switch (tokens[main_op].type) {
	  case '+': return val1 + val2;
	  case '-': if(single_operands)
				  return -val2;
				else return val1 - val2;
	  case '*': return val1 * val2;
	  case '/': if(val2 == 0)
				  panic("Divided by 0!");
				else return val1 / val2;
	  case TK_EQ: return val1 == val2;
	  default: panic("Bad operators!");
	}
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  
  printf("%d\n",eval(0, nr_token - 1));

  return 0;
}
