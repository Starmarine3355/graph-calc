#ifndef OPS_H
#define OPS_H

enum token_type {
	NO_TOK, // null-terminator
	NUMBER,
	OPERATOR,
	L_BRACKET,
	R_BRACKET
};

enum opr_type {
	UNARY,
	BINARY,
	MIX // both unary and binary operation (e.g -7 and 7-7)
};

struct token
{
	enum token_type type;

	// if number
	double value; 

	// if operator or function
	// as "symbol", trig. functions. use first letter of their name (e.g sin => 's') 
	// (inverse functions are capitalized, e.g asin => 'S')
	char symbol;
	int priority;
	enum opr_type arity; // number of arguments
};

/* returns 1 if number, returns 0 if not
 */
int is_number(char a);

/* returns 1 if operator (does not include functions), returns 0 if not
 */
int is_operator(char a);

/* returns correct symbol if function, returns value 1 if not
 */
char is_function(char func[MAX_BUF]);

/* returns order of operation (e.g sin is priority 4 meanwhile addition is priority 1)
 */
int get_priority(char opr);

/* returns "opr_type" for given operator symbol (e.g '*' is a BINARY operation)
 */
enum opr_type get_arity(char opr);

/* returns a number token after performing an operation of type "opr.type", uses "a" and "b" as operands
 */
struct token perform_opr(struct token opr, struct token a, struct token b);

#endif
