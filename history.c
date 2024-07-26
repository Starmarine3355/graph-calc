#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "history.h"

FILE* _get_history()
{
	FILE *f_history = fopen("history.bin", "rb+");

	if (f_history == NULL) // file does not exist
	{
		struct chunk null_chunk;
		null_chunk.type = NO_CHUNK;

		f_history = fopen("history.bin", "wb+");
		if (f_history == NULL)
		{
			fprintf(stderr, "In \"_get_history\": fopen() failed\n");
			exit(1);
		}
		if (fwrite(&null_chunk, sizeof(struct chunk), 1, f_history) == 0) // null-terminate history
		{
			fprintf(stderr, "In \"_get_history\": fwrite() failed\n");
			exit(1);
		}
		rewind(f_history); // point to before null-terminator
	}

	return f_history;
}

void _delete_excess_IO_pair(FILE *f_history)
{
	FILE *f_temp;
	struct chunk entries[MAX_STORAGE + 1];
	int file_size = ftell(f_history);
	int in_count = 0, out_count = 0;

	// if "history.bin" size is larger than MAX_STORAGE "chunks", delete one excess (oldest) IO pair
	if (file_size <= sizeof(struct chunk) * MAX_STORAGE)
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

	// since "save_to_history" always pushes one entry to "history.bin", we know there's one extra "chunk"
	if (!fread(&entries, sizeof(struct chunk), MAX_STORAGE + 1, f_history))
	{
		fprintf(stderr, "In \"_delete_excess_IO_pair\": fread() failed\n");
		exit(1);
	}
	for (int i = 0; i < MAX_STORAGE + 1; i++) // oldest entries are first, latest are last
	{
		// writes all "chunks" to "temp.bin" except for first INPUT and OUTPUT "chunk" 
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

		if (!fwrite(&entries[i], sizeof(struct chunk), 1, f_temp))
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

void save_to_history(enum chunk_type type, char string[MAX_BUF])
{
	FILE *f_history = _get_history();
	struct chunk entry[2]; // [0] is entry, [1] is new null chunk (previous is written over)

	entry[0].type = type;
	strcpy(entry[0].string, string);
	entry[1].type = NO_CHUNK;

	fseek(f_history, -(sizeof(struct chunk)), SEEK_END); // move pointer to null chunk
	if (!fwrite(&entry, sizeof(struct chunk), 2, f_history))
	{
		fprintf(stderr, "In \"save_to_history\": fwrite() failed\n");
		exit(1);
	}

	_delete_excess_IO_pair(f_history); // this also closes "f_history"
}

void copy_latest_IOs(enum chunk_type type, char dest[MAX_IO_LINES][MAX_BUF])
{
	if (type != INPUT && type != OUTPUT)
	{
		fprintf(stderr, "In \"copy_latest_IOs\": Wrong enum in \"type\"\n");
		exit(1);
	}

	FILE *f_history = _get_history();
	struct chunk entries[MAX_STORAGE];
	int findings = 0, i = 0;

	if (!fread(&entries, sizeof(struct chunk), MAX_STORAGE, f_history))
	{
		fprintf(stderr, "In \"copy_latest_IOs\": fread() failed\n");
		exit(1);
	}

	fclose(f_history);

	while (entries[i].type != NO_CHUNK)
		i++;
	if (i == 0)
		return; // history.bin only contains null-terminator

	for (i -= 1; i >= 0 && findings < MAX_IO_LINES; i--) // oldest are first, latest are last
	{
		if (entries[i].type == type)
		{
			strcpy(dest[findings], entries[i].string);
			findings++;
		}
	}
}

void copy_settings()
{

}

void print_help()
{
	FILE *f_help = fopen("help.txt", "r");
	if (f_help == NULL)
	{
		fprintf(stderr, "In \"print_help\": Couldn't open \"help.txt\"\n");
		exit(1);
	}

	//printf("%s");
}
