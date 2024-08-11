#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "commands.h"
#include "history.h"
#include "ops.h"

// modifies input string and removes all spaces
void _remove_spaces(char arg[MAX_BUF])
{
	char *p_char = arg;

	while (*arg != '\0')
	{
		if (*arg != ' ')
		{
			*p_char = *arg; // copies non-spaces to arg's memory address
			p_char++;
		}
		arg++;
	}

	*p_char = '\0'; // null termination
}

// prints out a tokens stack and queue to stderr, this allows a user to read the internal changes in _tokens_to_rpn and _calculate_rpn 
void _print_sq(char *msg, struct token stack[MAX_BUF], struct token queue[MAX_BUF])
{
	fprintf(stderr, "DEBUG: after %s:\n", msg);

	fprintf(stderr, "\tStack: ");
	for(int i = 0; stack[i].type != NO_TOK; i++)	
		fprintf(stderr, "{type: %d, symbol: %c, value: %.3f}\t", stack[i].type, stack[i].symbol, stack[i].value);
	
	if (queue[0].type != NO_TOK) // only output stack
	{
		fprintf(stderr, "\n\tQueue: ");
		for(int i = 0; queue[i].type != NO_TOK; i++)	
			fprintf(stderr, "{type: %d, symbol: %c, value: %.3f}\t", queue[i].type, queue[i].symbol, queue[i].value);
	}

	fprintf(stderr, "\n\n");
}

// push token to the top of dest
void _push(struct token dest[MAX_BUF], struct token tok)
{
	struct token null_tok = {NO_TOK};
	int i = 0;
	while (dest[i].type != NO_TOK)
		i++;

	dest[i] = tok; // move tok to top of dest
	dest[i + 1] = null_tok;
}

// pop (remove) and return token on the top of dest
struct token _pop(struct token dest[MAX_BUF])
{
	struct token null_tok = {NO_TOK};
	struct token ret_tok;
	int i = 0;
	while (dest[i].type != NO_TOK)
		i++;

	if (i == 0)
		return null_tok; // returns null token if stack is empty

	ret_tok = dest[i - 1]; 
	dest[i - 1] = null_tok; // pop token on the top of dest (null termination)
	return ret_tok; // return removed token
}

// arg is made into tokens structs (numbers, operators and brackets) and filled to tokens, the last index has a null token
struct result _tokenize(struct token tokens[MAX_BUF], char arg[MAX_BUF])
{
	int r_bracket_count = 0, l_bracket_count = 0, tok_count = 0, i = 0;
	while (arg[i] != '\0')
	{
		if (is_operator(arg[i]))
		{
			if (tok_count - 1 >= 0 && tokens[tok_count - 1].type == OPERATOR)
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
			int decimals = 0; // only one decimal is allowed
			int i_num = 1;
			char num_string[MAX_BUF] = {arg[i]}; // arg[i] is already confirmed to be a number
			
			if (arg[i] == '.')
				decimals++;

			while (1)
			{
				i++;

				if (is_number(arg[i]) || (arg[i] == '.' && decimals == 0))
				{
					num_string[i_num++] = arg[i]; // add next digit or decimal to number string
					if (arg[i] == '.')
						decimals++;
				}
				else if (arg[i] == '(' || (arg[i] >= 'a' && arg[i] <= 'z')) // hidden muliplication (e.g 5(3+2) --> 5*(3+2) or 5x --> 5*x)
				{
					if (arg[i + 1] != ')') // does not apply if brackets surround no numbers (e.g 5()+3)
					{
						tokens[tok_count + 1].type = OPERATOR;
						tokens[tok_count + 1].symbol = '*';
						tokens[tok_count + 1].priority = get_priority('*');
						tokens[tok_count + 1].assoc = get_assoc('*');
						tokens[tok_count + 1].arity = get_arity('*');

						tokens[tok_count].type = NUMBER;
						tokens[tok_count].value = atof(num_string);

						tok_count += 2; // increment with 2 because of the multiplication
					}
					else 
					{
						tokens[tok_count].type = NUMBER;
						tokens[tok_count].value = atof(num_string);

						tok_count++;
					}		

					i--; // the bracket will be skipped in the next iteration otherwise
					break;
				}
				else if (is_operator(arg[i]) || arg[i] == ')' || arg[i] == '\0')
				{
					tokens[tok_count].type = NUMBER;
					tokens[tok_count].value = atof(num_string);
					
					tok_count++;
					i--; // similar reason as above
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
		else // if function or variable
		{
			int i_func = 1;
			char func_string[MAX_BUF] = {arg[i]}; // arg[i] is start of function
			char symbol;

			// check for a single letter (variable)
			if ((arg[i] >= 'a' && arg[i] <= 'z') && (arg[i + 1] < 'a' || arg[i + 1] > 'z'))
			{
				fprintf(stderr, "variable %c = %.3f\n", arg[i], get_variable(arg[i]));
				tokens[tok_count].type = NUMBER;
				tokens[tok_count].value = get_variable(arg[i]);
			}
			else // no single letter means a function
			{
				while (1)  // function name ends with '(' (e.g sin(...) )
				{
					i++;
					if (arg[i] == '(')
						break;
					if (is_operator(arg[i]) || is_number(arg[i]) || arg[i] == ')' || arg[i] == '\0')
						return make_res(1, "ERROR in \"_tokenize\": Unexpected character when reading function");

					func_string[i_func++] = arg[i]; // fill until '(' is found
				}
	
				symbol = is_function(func_string);
				if (!symbol)
					return make_res(1, "ERROR in \"_tokenize\": Unknown function");
			
				tokens[tok_count].type = OPERATOR;
				tokens[tok_count].symbol = symbol;
				tokens[tok_count].priority = get_priority(symbol);
				tokens[tok_count].assoc = get_assoc(symbol);
				tokens[tok_count].arity = UNARY;

				i--; // similar reason as above
			}

			tok_count++;
		}

		i++;
	}

	// fill in missing right brackets (e.g 5(3(3+3 -> 5(3(3+3)) )
	for (i = 0; i < l_bracket_count - r_bracket_count; i++)
	{
		tokens[tok_count].type = R_BRACKET;
		tok_count++;
	}

	tokens[tok_count].type = NO_TOK; // null termination

	fprintf(stderr, "DEBUG: \"_tokenize\" is called with argument %s\n\tOutput: ", arg);
	for(int i = 0; tokens[i].type != NO_TOK; i++)	
		fprintf(stderr, "{type: %d, symbol: %c, value: %.3f}\t", tokens[i].type, tokens[i].symbol, tokens[i].value);
	fprintf(stderr, "\n\n");

	return make_res(0, "");
}

// modifies input token array so that its current expression is converted to reverse polish notation (RPN). function is inspired by the shunting yard algorithm. result.code is 0 if input is OK, else it is 1
struct result _tokens_to_rpn(struct token tokens[MAX_BUF])
{
	fprintf(stderr, "DEBUG: \"_tokens_to_rpn\" is called:\n");

	struct token null_tok = {NO_TOK};
	struct token opr_stack[MAX_BUF] = {null_tok};
	struct token out_queue[MAX_BUF] = {null_tok};
	int i = 0;

	while (tokens[i].type != NO_TOK)
	{
		if (tokens[i].type == NUMBER)
		{
			_push(out_queue, tokens[i]);
			_print_sq("number is pushed to queue", opr_stack, out_queue);
		}
		else if (tokens[i].type == OPERATOR)
		{
			int i_stk = 0;
			while (opr_stack[i_stk].type != NO_TOK)
				i_stk++; // find the top of the stack
			i_stk--; // do not read the null token

			while (i_stk >= 0 && opr_stack[i_stk].type == OPERATOR)
			{
				if (opr_stack[i_stk].priority < tokens[i].priority ||
				   (opr_stack[i_stk].priority == tokens[i].priority && opr_stack[i_stk].assoc == RIGHT))
					break;

				_push(out_queue, _pop(opr_stack)); // all operations of higher priority than tokens[i] (or same as tokens[i] but left associated) get popped from stack then pushed to the queue
				i_stk--; // traverse down the stack
				
				_print_sq("operator is popped from stack & pushed to queue", opr_stack, out_queue);
			}
			
			_push(opr_stack, tokens[i]);
			_print_sq("operator is pushed to stack", opr_stack, out_queue);
		}
		else if (tokens[i].type == L_BRACKET)
		{
			_push(opr_stack, tokens[i]);
			_print_sq("left bracket is pushed to stack", opr_stack, out_queue);
		}
		else if (tokens[i].type == R_BRACKET)
		{
			int i_stk = 0;
			while (opr_stack[i_stk].type != NO_TOK)
				i_stk++; // find the top of the stack
			i_stk--; // do not read the null token

			while (i_stk >= 0 && opr_stack[i_stk].type != L_BRACKET)
			{
				_push(out_queue, _pop(opr_stack));
				i_stk--;
				_print_sq("operator between brackets is popped from stack & pushed to queue", opr_stack, out_queue);
			}

			if (opr_stack[i_stk].type != L_BRACKET)
				return make_res(1, "ERROR in \"_tokens_to_rpn\": Did not find left bracket");

			_pop(opr_stack); // brackets are not needed in an rpn expression
			_print_sq("left bracket is popped from stack", opr_stack, out_queue);
		}
		else
			return make_res(1, "ERROR in \"_tokens_to_rpn\": Unexpected token");
		i++;
	}

	i = 0;
	while (opr_stack[i].type != NO_TOK)
		i++; // find the top of the stack
	while (i >= 0)
		_push(out_queue, opr_stack[i--]); // remaining operations in the stack get moved to queue

	for (int i = 0; i < MAX_BUF; i++)
		tokens[i] = out_queue[i]; // move tokens in queue to input array
	
	fprintf(stderr, "DEBUG: \"_tokens_to_rpn\" is finished:\n\tOutput: ");
	for(int i = 0; tokens[i].type != NO_TOK; i++)	
		fprintf(stderr, "{type: %d, symbol: %c, value: %.3f}\t", tokens[i].type, tokens[i].symbol, tokens[i].value);
	fprintf(stderr, "\n\n");

	return make_res(0, ""); // input is OK
}

// TODO: might be public with plotting feature
// takes input array with tokens and calculates its expression (this must be written in RPN). the first token in the input array (tokens[0]) is set to the expression's value. result.code is 0 if input is OK, else it is 1
struct result _calculate_rpn(struct token tokens[MAX_BUF])
{
	fprintf(stderr, "DEBUG: \"_calculate_rpn\" is called:\n");

	struct token null_tok = {NO_TOK};
	struct token empty_queue[MAX_BUF] = {null_tok}; // for debugging purposes
	struct token out_stack[MAX_BUF] = {null_tok};
	int i = 0;

	while (tokens[i].type != NO_TOK)
	{
		if (tokens[i].type == NUMBER)
		{
			_push(out_stack, tokens[i]);
			_print_sq("number is pushed to stack", out_stack, empty_queue);
		}
		else if (tokens[i].type == OPERATOR)
		{
			struct token res, a, b;
			b = _pop(out_stack);
			if (b.type == NO_TOK)
				return make_res(1, "ERROR in \"_calculate_rpn\": Number expected after an operator");

			if (tokens[i].arity != UNARY) // operator takes two operands (or is mixed)
			{
				a = _pop(out_stack);
				if (a.type == NO_TOK && tokens[i].arity != MIX)
					return make_res(1, "ERROR in \"_calculate_rpn\": Two numbers expected after a binary operator");
			}

			res = perform_opr(tokens[i], a, b);
			if (res.type == NO_TOK)
				return make_res(1, "ERROR in \"_calculate_rpn\": Unexpected operator");
				
			_push(out_stack, res); 

			char dmsg[64];
			sprintf(dmsg, "'%c' operator (arity type: %d) is performed on stack", tokens[i].symbol, tokens[i].arity);
			_print_sq(dmsg, out_stack, empty_queue);
		}
		else
			return make_res(1, "ERROR in \"_calculate_rpn\": Unexpected token");

		i++;
	}

	fprintf(stderr, "DEBUG: \"_calculate_rpn\" is finished:\n\tOutput: %f\n\n", out_stack[0].value);
	
	tokens[0] = out_stack[0];
	return make_res(0, "");
}

// read .h file
struct result process_input(char _input[MAX_BUF])
{
	char input[MAX_BUF];
	strcpy(input, _input); // strtok modifies input string

	char option[MAX_BUF], arg[MAX_BUF], output[MAX_BUF];
	struct token tokens[MAX_BUF];
	struct result res;

	char *input_tok = strtok(input, " ");
	if (input_tok != NULL)
	{
		strcpy(option, input_tok);
		input_tok = strtok(NULL, "");
		if (input_tok != NULL)
			strcpy(arg, input_tok);
	}
	else
		strcpy(option, _input);

	if (strcmp(option, "calc") == 0)
	{
		_remove_spaces(arg);

		res = _tokenize(tokens, arg); 
		if (res.code)
			return res;

		res = _tokens_to_rpn(tokens);
		if (res.code)
			return res;

		res = _calculate_rpn(tokens);
		if (res.code)
		return res;

		set_variable('a', tokens[0].value); // set answer variable
		sprintf(output, "%f", tokens[0].value);
		res = make_res(0, output);
	} 
	else if (strcmp(option, "plot") == 0)
	{
		res = make_res(1, "plotting");
		// TODO
	}
	else if (strcmp(option, "load") == 0)
	{
		char name = arg[0];

		strtok(arg, " "); // e.g load a 5, 'a' is skipped so that the next call returns '5'
		char *value = strtok(NULL, " ");

		if (name < 'a' || name > 'z')
			return make_res(1, "ERROR in \"process_input\": Variable is not lowercase or not from the English alphabet.");
		
		set_variable(name, atof(value));
		sprintf(output, "%.3f --> %c", atof(value), name);
		res = make_res(1, output);
	}
	else if (strcmp(option, "help") == 0)
		res = make_res(1, "Opening \"help\"");
	else if (strcmp(option, "quit") == 0)
		res = make_res(1, "Opening \"main\"");
	else if (strcmp(option, "his") == 0)
		res = make_res(1, "Opening \"history\"");
	else
		res = make_res(1, "ERROR in \"process_input\": Not a valid option. Type \"help\" for more info.");
	
	return res;
}
