#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "history.h"
#include "display.h"

// writes default settings to and null terminates file
void _initialise_history(FILE *f_history)
{
	struct entry null_entry;
	struct entry variables[26];

	for (int i = 0; i < 26; i++)
	{
		variables[i].type = VARIABLE;
		variables[i].string[0] = 'a' + i;
		variables[i].value = 0; // default value
	}
	if (fwrite(variables, sizeof(struct entry), 26, f_history) == 0)
	{
		fprintf(stderr, "In \"_initialise_history\": fwrite() failed\n");
		exit(1);
	}

	null_entry.type = NO_ENTRY;
	if (fwrite(&null_entry, sizeof(struct entry), 1, f_history) == 0) // null termination
	{
		fprintf(stderr, "In \"_initialise_history\": fwrite() failed\n");
		exit(1);
	}
	
	rewind(f_history); // point to start of file
}

// opens or creates history.bin and returns its file pointer
FILE* _get_history()
{
	FILE *f_history = fopen("history.bin", "rb+");
	if (f_history == NULL) // file does not exist
	{
		struct entry null_entry;
		null_entry.type = NO_ENTRY;

		f_history = fopen("history.bin", "wb+");
		if (f_history == NULL)
		{
			fprintf(stderr, "In \"_get_history\": fopen() failed\n");
			exit(1);
		}

		_initialise_history(f_history);
	}

	return f_history;
}

// if history.bin has more than MAX STORAGE entries, the oldest pair of input and output is removed
void _delete_excess_IO_pair(FILE *f_history)
{
	FILE *f_temp;
	struct entry entries[MAX_STORAGE + 1];
	int file_size = ftell(f_history);
	int in_count = 0, out_count = 0;

	if (file_size <= sizeof(struct entry) * MAX_STORAGE)
	{
		fclose(f_history);
		return;
	}

	f_temp = fopen("temp.bin", "wb");
	if (f_temp == NULL)
	{
		fprintf(stderr, "In \"_delete_excess_IO_pair\": fopen() failed\n");
		exit(1);
	}
	
	rewind(f_history);

	// since save_to_history writes one entry at a time, we know there's one extra entry in history.bin
	if (!fread(&entries, sizeof(struct entry), MAX_STORAGE + 1, f_history))
	{
		fprintf(stderr, "In \"_delete_excess_IO_pair\": fread() failed\n");
		exit(1);
	}
	for (int i = 0; i < MAX_STORAGE + 1; i++) // oldest entries are first, youngest are last
	{
		// write all entries apart from first IO pair to temp file
		if (entries[i].type == INPUT && in_count == 0)
		{
			in_count++;
			continue;
		}
		else if (entries[i].type == OUTPUT && out_count == 0)
		{
			out_count++;
			continue;
		}

		if (!fwrite(&entries[i], sizeof(struct entry), 1, f_temp))
		{
			fprintf(stderr, "In \"_delete_excess_IO_pair\": fwrite() failed\n");
			exit(1);
		}
	}
	
	fclose(f_history);
	fclose(f_temp);

	if (remove("history.bin") != 0)
	{
		fprintf(stderr, "In \"_delete_excess_IO_pair\": remove() failed\n");
		exit(1);
	}
	if (rename("temp.bin", "history.bin") != 0)
	{
		fprintf(stderr, "In \"_delete_excess_IO_pair\": rename() failed\n");
		exit(1);
	}
}

void print_history()
{
	FILE *f_history = _get_history();
	int input_count = 0, output_count = 0, var_count = 0;
	struct entry read;
	struct entry inputs[MAX_STORAGE] = {NO_ENTRY}, outputs[MAX_STORAGE] = {NO_ENTRY}, variables[MAX_STORAGE] = {NO_ENTRY};

	while (1)
	{
		if (!fread(&read, sizeof(struct entry), 1, f_history))
		{
			fprintf(stderr, "In \"print_history\": fread() failed\n");
			exit(1);
		}
		
		if (read.type == NO_ENTRY)
			break;
		else if (read.type == VARIABLE)
			variables[var_count++] = read;
		else if (read.type == INPUT)
			inputs[input_count++] = read;
		else if (read.type == OUTPUT)
			outputs[output_count++] = read;
	}
	fclose(f_history);
	
	// inputs and outputs (both has the same amount)
	for (int i = 0; i < input_count; i++)
	{
		printf("| (%d) %s\n", input_count - i, inputs[i].string);
		printf("| (~) %s\n", outputs[i].string);
	}
	print_line();

	// variables
	for (int i = 0; i < var_count; i += 3)
	{
		printf("| ");
		for (int j = 0; j < 3; j++)
		{
			int name = 'a' + i + j, i_var = 0;
			if (name > 'z')
				break;
			while (variables[i_var].string[0] != name)
				i_var++;

			printf("%.2f --> %c\t\t\t", variables[i_var].value, variables[i_var].string[0]);
		}
		printf("\n");
	}
}

void save_to_history(enum entry_type type, char string[MAX_BUF], double value)
{
	FILE *f_history = _get_history();
	struct entry entries[2]; // [0] is entry, [1] is new null terminator

	entries[0].type = type;
	entries[0].value = value;
	strcpy(entries[0].string, string);
	entries[1].type = NO_ENTRY;

	fseek(f_history, -sizeof(struct entry), SEEK_END); // moves to before null terminator

	if (!fwrite(&entries, sizeof(struct entry), 2, f_history))
	{
		fprintf(stderr, "In \"save_to_history\": fwrite() failed\n");
		exit(1);
	}

	_delete_excess_IO_pair(f_history); // this closes the file as well
}

void copy_latest_IOs(enum entry_type type, char dest[MAX_IO_LINES][MAX_BUF])
{
	FILE *f_history = _get_history();
	struct entry entries[MAX_STORAGE];
	int findings = 0, i = 0;

	if (!fread(&entries, sizeof(struct entry), MAX_STORAGE, f_history))
	{
		fprintf(stderr, "In \"copy_latest_IOs\": fread() failed\n");
		exit(1);
	}
	fclose(f_history);

	while (entries[i].type != NO_ENTRY)
		i++;
	for (i -= 1; i >= 0 && findings < MAX_IO_LINES; i--) // oldest entries are first, youngest are last
	{
		if (entries[i].type == type)
			strcpy(dest[findings++], entries[i].string);
	}
}

double get_variable(char name)
{
	FILE *f_history = _get_history();
	struct entry entries[MAX_STORAGE];
	int i = 0;

	if (!fread(&entries, sizeof(struct entry), MAX_STORAGE, f_history))
	{
		fprintf(stderr, "In \"get_variable\": fread() failed\n");
		exit(1);
	}
	fclose(f_history);

	fprintf(stderr, "get_variable\n");
	while(entries[i].type != NO_ENTRY)
	{
		if (entries[i].type == VARIABLE && entries[i].string[0] == name)
			break;
		fprintf(stderr, "\t(%d, %c, %.3f)\n", entries[i].type, entries[i].string[0], entries[i].value);
		i++;
	}
	return entries[i].value;
}

void set_variable(char _name, double value)
{
	FILE *f_history = _get_history(), *f_temp;
	char name[MAX_BUF] = {_name, '\0'};
	struct entry entries[MAX_STORAGE];

	f_temp = fopen("temp.bin", "wb");
	if (f_temp == NULL)
	{
		fprintf(stderr, "In \"set_variable\": fopen() failed\n");
		exit(1);
	}

	if (!fread(&entries, sizeof(struct entry), MAX_STORAGE, f_history))
	{
		fprintf(stderr, "In \"set_variable\": fread() failed\n");
		exit(1);
	}
	for (int i = 0; i < MAX_STORAGE; i++)
	{
		// write all entries apart from the old variable entry to temp.bin
		if (entries[i].type == VARIABLE && entries[i].string[0] == name[0])
			continue;
		if (!fwrite(&entries[i], sizeof(struct entry), 1, f_temp))
		{
			fprintf(stderr, "In \"set_variable\": fwrite() failed\n");
			exit(1);
		}
		if (entries[i].type == NO_ENTRY)
			break;
	}
	
	fclose(f_history);
	fclose(f_temp);

	if (remove("history.bin") != 0)
	{
		fprintf(stderr, "In \"set_variable\": remove() failed\n");
		exit(1);
	}
	if (rename("temp.bin", "history.bin") != 0)
	{
		fprintf(stderr, "In \"set_variable\": rename() failed\n");
		exit(1);
	}

	save_to_history(VARIABLE, name, value);  
}
