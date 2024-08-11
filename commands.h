#ifndef COMMANDS_H
#define COMMANDS_H

/* takes in input string and returns message as result struct, input is formatted as `option argument`
 * after navigation command (help/quit/his) a message is returned (display mode is changed in main.c)
 * after calculation command (calc/plot) the argument is evaluated. the process is, tokenize -> convert expression to RPN -> evaluate -> print result or draw graph
 *
 * result.code is 0 if output should be saved to history.bin and otherwise it is 1
 */
struct result process_input(char _input[MAX_BUF]);

#endif
