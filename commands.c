#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "ops.h"

void _remove_spaces(char arg[MAX_BUF])
{
	char *p_char = arg;

	while (*arg != '\0')
	{
		if (*arg != ' ') // copies non-spaces, thus spaces are replaced or after null-terminator
		{
			*p_char = *arg;
			p_char++;
		}
		arg++; // "arg" is passed by value, does not modify original pointer
	}

	*p_char = '\0';
}

void _replace_vars(char arg[MAX_BUF])
{
	// wip
}

void _push(struct token dest[MAX_BUF], struct token tok)
{
	struct token null_tok = {NO_TOK};
	int i = 0;
	while (dest[i].type != NO_TOK)
		i++;

	dest[i] = tok; // move "tok" to top of "dest"
	dest[i + 1] = null_tok;
}

struct token _pop(struct token dest[MAX_BUF])
{
	struct token null_tok = {NO_TOK};
	struct token ret_tok;
	int i = 0;
	while (dest[i].type != NO_TOK)
		i++;

	if (i == 0)
		return null_tok; // returns null-token if stack is empty

	ret_tok = dest[i - 1]; 
	dest[i - 1] = null_tok; // pop token on the top of "dest"
	return ret_tok; // return removed token
}
struct result _tokenize(struct token tokens[MAX_BUF], char arg[MAX_BUF])
{
	fprintf(stderr, "RUN TOKENIZE:\t%s\n", arg);
	

	int r_bracket_count = 0, l_bracket_count = 0, tok_count = 0, i = 0;
	while (arg[i] != '\0')
	{
		if (is_operator(arg[i]))
		{
			if (tokens[tok_count - 1].type == OPERATOR)
				return make_res(1, "ERROR in \"_tokenize\": Two operators cannot be next to each other");
			
			tokens[tok_count].type = OPERATOR;
			tokens[tok_count].symbol = arg[i];
			tokens[tok_count].priority = get_priority(arg[i]);
			tokens[tok_count].assoc = get_assoc(arg[i]);
			tokens[tok_count].arity = get_arity(arg[i]);

			tok_count++;
		}
		else if (is_number(arg[i]) || arg[i] == '.')
		{
			int decimals = 0;
			int i_num = 1;
			char a_num[MAX_BUF] = {arg[i]};
			
			if (arg[i] == '.')
				decimals++;

			while (1)
			{
				i++;

				if (is_number(arg[i]) || (arg[i] == '.' && decimals == 0))
				{
					a_num[i_num++] = arg[i]; // add next digit to number string
					if (arg[i] == '.') // only 1 decimal point allowed
						decimals++;
				}
				else if (arg[i] == '(') // "hidden" '*' before brackets (e.g 5(3+2) => 5*(3+2))
				{
					if (arg[i + 1] != ')') // does not apply if brackets surround no numbers (e.g 5()+3)
					{
						tokens[tok_count + 1].type = OPERATOR;
						tokens[tok_count + 1].symbol = '*';
						tokens[tok_count + 1].priority = get_priority('*');
						tokens[tok_count].assoc = get_assoc('*');
						tokens[tok_count + 1].arity = get_arity('*');

						tokens[tok_count].type = NUMBER;
						tokens[tok_count].value = atof(a_num); // convert number string

						tok_count += 2; // extra '*' operator is added
					}
					else 
					{
						tokens[tok_count].type = NUMBER;
						tokens[tok_count].value = atof(a_num); // convert number string

						tok_count++;
					}		

					i--;
					break;
				}
				else if (is_operator(arg[i]) || arg[i] == ')' || arg[i] == '\0')
				{
					tokens[tok_count].type = NUMBER;
					tokens[tok_count].value = atof(a_num); // convert number string
					
					tok_count++;
					i--;
					break;
				}
				else
					return make_res(1, "ERROR in \"_tokenize\": Unexpected character after number");
			}
		}
		else if (arg[i] == '(')
		{
			tokens[tok_count++].type = L_BRACKET;
			l_bracket_count++;
		}
		else if (arg[i] == ')')
		{
			tokens[tok_count++].type = R_BRACKET;
			r_bracket_count++;
		}
		else
		{
			int i_opr = 1;
			char a_opr[MAX_BUF] = {arg[i++]};
			char symbol;

			while (1)
			{
				if (arg[i] == '(') // function name ends with '(' (e.g sin(...) )
					break;
				else if (is_operator(arg[i]) || is_number(arg[i]) ||
					 arg[i] == ')' || arg[i] == '\0')
				{
					return make_res(1, "ERROR in \"_tokenize\": Unexpected character when reading function");
				}

				a_opr[i_opr++] = arg[i++]; // fill until '(' is found
			}

			symbol = is_function(a_opr);
			if (symbol == 1)
				return make_res(1, "ERROR in \"_tokenize\": Unknown function");
			
			tokens[tok_count].type = OPERATOR;
			tokens[tok_count].symbol = symbol;
			tokens[tok_count].priority = get_priority(symbol);
			tokens[tok_count].assoc = get_assoc(symbol);
			tokens[tok_count].arity = UNARY;
			
			tok_count++;
			i--;
		}

		i++;
	}

	// fill in missing right brackets (e.g "5(3(3+3" => "5(3(3+3))" )
	for (i = 0; i < l_bracket_count - r_bracket_count; i++)
	{
		tokens[tok_count].type = R_BRACKET;
		tok_count++;
	}

	tokens[tok_count].type = NO_TOK; // "null_terminate" 


	for(int i = 0; tokens[i].type != NO_TOK; i++)	
		fprintf(stderr, "Token %d:\tsymbol: %c | value: %f\n", tokens[i].type, tokens[i].symbol, tokens[i].value);
	fprintf(stderr, "END TOKENIZE\n");
	fprintf(stderr, "\n");

	return make_res(0, "");
}


// look up "shunting yard algorithm" (video in README.md) (rpn stands for reverse polish notation)
struct result _tokens_to_rpn(struct token tokens[MAX_BUF])
{
	fprintf(stderr, "RUN TOKENS TO RPN:\n");

	struct token null_tok = {NO_TOK};
	struct token opr_stack[MAX_BUF] = {null_tok};
	struct token out_queue[MAX_BUF] = {null_tok};
	int i = 0;

	while (tokens[i].type != NO_TOK)
	{
		if (tokens[i].type == NUMBER)
			_push(out_queue, tokens[i]); // get number to queue
		else if (tokens[i].type == OPERATOR)
		{
			int i_stk = 0;
			while (opr_stack[i_stk].type != NO_TOK)
				i_stk++; // find the top of the stack
			i_stk--; // do not read the null token

			while (opr_stack[i_stk].type == OPERATOR)
			{
				if (opr_stack[i_stk].priority < tokens[i].priority ||
				   (opr_stack[i_stk].priority == tokens[i].priority && opr_stack[i_stk].assoc == RIGHT))
					break;

				_push(out_queue, _pop(opr_stack)); // all ops of higher priority than tokens[i] (or same as tokens[i] but left associated) get _pushed to queue (and _popped from stack)
				i_stk--; // traverse down the stack
			}
			
			_push(opr_stack, tokens[i]); // get operator to stack
		}
		else if (tokens[i].type == L_BRACKET)
			_push(opr_stack, tokens[i]); // get left bracket to stack
		else if (tokens[i].type == R_BRACKET)
		{
			int i_stk = 0;
			while (opr_stack[i_stk].type != NO_TOK)
				i_stk++; // find the top of the stack
			i_stk--; // do not read the null token

			while (opr_stack[i_stk].type != L_BRACKET && i_stk >= 0)
			{
				_push(out_queue, _pop(opr_stack)); // ops between brackets get _pushed to queue
				i_stk--; // traverse down the stack
			}

			if (opr_stack[i_stk].type != L_BRACKET)
				return make_res(1, "ERROR in \"_tokens_to_rpn\": Did not find left bracket");

			_pop(opr_stack); // brackets are not needed in rpn
		}
		else
			return make_res(1, "ERROR in \"_tokens_to_rpn\": Unexpected token");
		i++;
	}

	i = 0;
	while (opr_stack[i].type != NO_TOK)
	{
		_push(out_queue, opr_stack[i]); // remaining ops get moved to queue
		i++;
	}

	for (int i = 0; i < MAX_BUF; i++)
		tokens[i] = out_queue[i]; // move queue tokens to "tokens"
	
	for(int i = 0; tokens[i].type != NO_TOK; i++)	
		fprintf(stderr, "Token %d:\tsymbol: %c | value: %f\n", tokens[i].type, tokens[i].symbol, tokens[i].value);
	fprintf(stderr, "END TOKENS TO RPN\n");
	fprintf(stderr, "\n");

	return make_res(0, "");
}

	/* ERRORS to check
	 *	* L_Brackets with out R_Bracket (e.g "5(32(4") (Is feature, skip tokens of type L_Bracket)
	 */

struct result _calculate_rpn(struct token tokens[MAX_BUF])
{
	for(int i = 0; tokens[i].type != NO_TOK; i++)	
		fprintf(stderr, "Token %d:\tsymbol: %c | value: %f\n", tokens[i].type, tokens[i].symbol, tokens[i].value);
	fprintf(stderr, "\n");

	struct token null_tok = {NO_TOK};
	struct token out_stack[MAX_BUF] = {null_tok};
	struct result res;
	int i = 0;

	while (tokens[i].type != NO_TOK)
	{
		fprintf(stderr, "NEW Token %d:\tsymbol: %c | value: %f\n", tokens[i].type, tokens[i].symbol, tokens[i].value);

		if (tokens[i].type == NUMBER)
			_push(out_stack, tokens[i]);
		else if (tokens[i].type == OPERATOR)
		{
			struct token res, a, b; // b is first on the stack, a is second. thus b is popped first (e.g 5-3 => 3, 5, -) 
			b = _pop(out_stack); // if _pop can't pop, it returns null token
			if (b.type == NO_TOK)
				return make_res(1, "ERROR in \"_calculate_rpn\": Number expected after an operator");

			fprintf(stderr, "Opr arity is %d: UNARY is %d\n", tokens[i].arity, UNARY);
			if (tokens[i].arity != UNARY) // i.e BINARY and MIX (UNARY and BINARY opr)
			{
				a = _pop(out_stack);
				fprintf(stderr, "token a is %f\n", a.value);
				if (a.type == NO_TOK && tokens[i].arity != MIX)
					return make_res(1, "ERROR in \"_calculate_rpn\": Two numbers expected after a binary operator");
			}

			res = perform_opr(tokens[i], a, b);
			if (res.type == NO_TOK)
				return make_res(1, "ERROR in \"_calculate_rpn\": Unexpected operator");
				
			fprintf(stderr, "Result after '%c' opr: %f\n", tokens[i].symbol, res.value);
			_push(out_stack, res); 
		}
		else
			return make_res(1, "ERROR in \"_calculate_rpn\": Unexpected token");

		i++;
	}
	
	fprintf(stderr, "Switching [0]s, ");
	fprintf(stderr, "Token %d:\tsymbol: %c | value: %f\n", tokens[0].type, tokens[0].symbol, tokens[0].value);

	tokens[0] = out_stack[0];
	res.code = 0;
	return res;
}

struct result process_input(char _input[MAX_BUF])
{
	char input[MAX_BUF];
	strcpy(input, _input); // strtok modifies input string

	char option[MAX_BUF], arg[MAX_BUF], output[MAX_BUF];
	struct token tokens[MAX_BUF];
	struct result res;

	char *tok = strtok(input, " ");
	if (tok != NULL)
	{
		strcpy(option, tok);
		tok = strtok(NULL, "\"");
		if (tok != NULL)
			strcpy(arg, tok);
	}
	else
		strcpy(option, _input);

	if (strcmp(option, "calc") == 0)
	{
		// move to future calc function
		_remove_spaces(arg);
		_replace_vars(arg); // wip, important that SINGLE chars get converted (sin is a func)

		res = _tokenize(tokens, arg); 
		if (res.code)
			return res;

		res = _tokens_to_rpn(tokens);
		if (res.code)
			return res;

		res = _calculate_rpn(tokens); // modifies tokens, [0] is final value (NUMBER token)
		if (res.code)
			return res;

		sprintf(output, "%f", tokens[0].value);
		res = make_res(0, output);
	} 
	else if (strcmp(option, "plot") == 0)
		res = make_res(0, "plotting");
	else if (strcmp(option, "help") == 0)
		res = make_res(0, "Opening \"help\"");
	else if (strcmp(option, "quit") == 0)
		res = make_res(0, "Opening \"main\"");
	else if (strcmp(option, "config") == 0)
		res = make_res(0, "Opening \"config\"");
	else
		res = make_res(1, "ERROR in \"process_input\": Not a valid option. Type \"help\" for more info.");
	
	return res;
}
