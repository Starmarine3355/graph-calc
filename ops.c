#include <stdio.h>
#include <string.h>
#include <math.h>
#include "macros.h"
#include "ops.h"

int is_number(char a)
{
	return a - 48 >= 0 && a - 48 <= 9; // ASCII value for '0' is 48
}

int is_operator(char a)
{
	return a == '+' || a == '-' ||
	       a == '*' || a == '/' ||
	       a == '^' || a == '#';
}

char is_function(char func[MAX_BUF])
{
	if (strcmp(func, "sin") == 0)
		return 's';
	if (strcmp(func, "asin") == 0)
		return 'S';
	if (strcmp(func, "cos") == 0)
		return 'c';
	if (strcmp(func, "acos") == 0)
		return 'C';
	if (strcmp(func, "tan") == 0)
		return 't';
	if (strcmp(func, "atan") == 0)
		return 'T';
	return 1; // could not find function
}

int get_priority(char opr)
{
	if (opr == '+' || opr == '-')
		return 1;
	if (opr == '*' || opr == '/')
		return 2;
	if (opr == '^' || opr == '#')
		return 3;
	return 4; // functions (e.g sin)
}

enum associativity get_assoc(char opr)
{
	if (opr == '+' || opr == '-' ||
	    opr == '*' || opr == '/')
		return LEFT;
	return RIGHT; // exponentiation, roots and functions
}

enum opr_type get_arity(char opr)
{
	if (opr == '+' || opr == '-')
		return MIX;
	else if (opr == '*' || opr == '/' || opr == '^' || opr == '#')
		return BINARY;
	else
		return UNARY; // functions (e.g sin(90) takes 90 as argument)
}

struct token perform_opr(struct token opr, struct token a, struct token b)
{
	// no error check, if a is num and b is num use them. opr is always believed to be correct
	
	struct token result = {NUMBER, 0.0};
	if (a.type == NO_TOK)
		a.value = 0.0; // only matters for mixed operations

	fprintf(stderr, "Operands are: %f\t%f\n", a.value, b.value);
	switch (opr.symbol)
	{
		case '+':
			result.value = a.value + b.value;
			break;
		case '-':
			result.value = a.value - b.value;
			break;
		case '*':
			result.value = a.value * b.value;
			break;
		case '/':
			result.value = a.value / b.value;
			break;
		case '^':
			result.value = pow(a.value, b.value);
			break;
		case '#':
			result.value = pow(a.value, 1 / b.value);
			break;
		case 's':
			result.value = sin(b.value); 
			break;
		case 'S':
			result.value = asin(b.value);
			break;
		case 'c':
			result.value = cos(b.value); 
			break;
		case 'C':
			result.value = acos(b.value);
			break;
		case 't':
			result.value = tan(b.value); 
			break;
		case 'T':
			result.value = atan(b.value);
			break;
		default:
			result.type = NO_TOK;
			break;
	}

	return result;
}
