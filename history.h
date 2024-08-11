#ifndef HISTORY_H
#define HISTORY_H

// 32 inputs, 32 outputs, 26 variables, 1 null terminator
#define MAX_STORAGE 91

enum entry_type {
	NO_ENTRY, // null terminator
	INPUT,
	OUTPUT,
	VARIABLE,
};

struct entry {
	enum entry_type type;
	char string[MAX_BUF];
	double value;
};

/* prints history.bin's contents
 */
void print_history();

/* creates an entry struct with input arguments and saves it to history.bin
 * if history.bin has more than MAX STORAGE entries, the oldest pair of input and output is removed.
 */
void save_to_history(enum entry_type type, char string[MAX_BUF], double value);

/* copies latest 5 input or output strings to array dest (does not accept other entry types)
 */
void copy_latest_IOs(enum entry_type type, char dest[MAX_IO_LINES][MAX_BUF]);

/* sets variable to value
 */
void set_variable(char _name, double value);

/* gets variable's value
 */
double get_variable(char name);

#endif
