#ifndef HISTORY_H
#define HISTORY_H

#define MAX_STORAGE 65 // 32 IOs, 0 Settings, 1 NO_CHUNK

enum chunk_type {
	NO_CHUNK, // null-terminator
	INPUT,
	OUTPUT,
	SETTING
};

struct chunk {
	enum chunk_type type;
	char string[MAX_BUF];
};

/* saves string to history.bin.
 * "chunk_type" specifies if string is input, output or a setting.
 * this makes reading possible.
 *
 * side effect: if storage is > MAX_STORAGE, oldest pair of INPUT and OUTPUT entries is removed.
 */
void save_to_history(enum chunk_type type, char string[MAX_BUF]);

/* copies latest entries of inputs or outputs to dest
 * only allows INPUT and OUTPUT enum
 */
void copy_latest_IOs(enum chunk_type type, char dest[MAX_IO_LINES][MAX_BUF]);

// todo copy_settings

/* opens help.txt and prints its contents to the terminal.
 * exits program if help.txt is not in executable's directory
 */
void print_help();

#endif
