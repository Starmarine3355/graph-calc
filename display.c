#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "history.h"
#include "display.h"

// erases screen using escape sequence
void _erase_screen()
{
	printf("%c[2J", 27);
}

// prints help.txt's contents, if the file is not found the program is exited
void _print_help()
{
	char c;
	FILE *f_help = fopen("help.txt", "r");
	if (f_help == NULL)
	{
		fprintf(stderr, "In \"print_help\": Couldn't open \"help.txt\"\n");
		exit(1);
	}

	while ((c = getc(f_help)) != EOF)
		putchar(c);

	printf("\n");
	fclose(f_help);
}

void print_line()
{
	for (int i = 0; i < WIDTH; i++)
		printf("_");
	printf("\n\n");
}

void draw_axis(struct graph *p_plot)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == HEIGHT / 2 + 1)
				p_plot->stream[i][j] = '-'; // x-axis
			if (j == WIDTH / 2 + 1)
				p_plot->stream[i][j] = '|'; // y-axis
		}
	}

	p_plot->stream[HEIGHT / 2 + 1][WIDTH / 2 + 1] = '+'; // origo
}

void reset_plot(struct graph *p_plot)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			p_plot->stream[i][j] = ' ';
		}
	}
}

/* TODO:
 *	display different modes
 *	normal
 *	help
 *	config
 *
 */


struct result display(enum display_mode mode, struct graph plot, char inputs[MAX_IO_LINES][MAX_BUF], char outputs[MAX_IO_LINES][MAX_BUF])
{
	struct result res;
	res.code = 0;

	_erase_screen();

	switch (mode)
	{
		case NORMAL:
			// plot graph
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					printf("%c", plot.stream[i][j]);
				}
				printf("\n");
			}

			// print previous IOs
			print_line();
			for (int i = MAX_IO_LINES - 1; i >= 0; i--)
			{
				printf("| (%d) %s\n", i + 1, inputs[i]);
				printf("| (~) %s\n", outputs[i]);
			}
			print_line();
					
			break;
		case HELP:
			print_line();
			_print_help();
			print_line();
			break;
		case HISTORY:
			print_line();
			print_history();
			print_line();
			break;
	}

	// iterate IOs
	for (int i = MAX_IO_LINES - 1; i >= 0; i--)
	{
		strcpy(inputs[i], inputs[i - 1]);
		strcpy(outputs[i], outputs[i - 1]);
	}

	// print prompt	
	printf("(P: %.3f, %.3f) (S: %.3f) >> ", plot.pos.x, plot.pos.y, plot.scale);
	fgets(inputs[0], MAX_BUF, stdin);

	// checks if input is less than MAX_BUF characters long
	if (strchr(inputs[0], '\n'))
		inputs[0][strcspn(inputs[0], "\n")] = '\0';
	else
	{
		// consume until/and newline
		// %*... = don't save, [^\n] = until newline
		scanf("%*[^\n]");
		scanf("%*c");
		
		res.code = 1;
		strcpy(res.msg, "ERROR in \"display\": User input is longer than MAX_BUF. Type \"help\" for more info."); 
	}

	return res;
}
