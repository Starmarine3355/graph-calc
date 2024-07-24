#include <stdio.h>
#include <string.h>
#include "display.h"
#include "history.h"
#include "commands.h"
#include "macros.h"

void update(void);

int main(void)
{
	update();
	return 0;
}

void update(void)
{
	struct result res;

	struct graph plot = {.scale = 1.5, .pos = {1000, 0}}; // load from history

	char inputs[MAX_IO_LINES][MAX_BUF] = {"", "", "", "", ""};
	char outputs[MAX_IO_LINES][MAX_BUF] = {"", "", "", "", ""};

	copy_latest_IOs(INPUT, inputs);
	copy_latest_IOs(OUTPUT, outputs);

	while (1)
	{
		reset_plot(&plot);
		draw_axis(&plot);

		res = display(plot, inputs, outputs);
		if (res.code == 1)
		{
			fprintf(stderr, "Error in display: %s\n", res.msg);
			strcpy(outputs[0], res.msg); 
			continue;
		}
		
		res = process_input(inputs[0]);
		strcpy(outputs[0], res.msg);
		if (res.code == 0)
		{
			save_to_history(OUTPUT, outputs[0]);
			save_to_history(INPUT, inputs[0]);
		}

		// todo return result structs
		if (strcmp(inputs[0], "help") == 0)
			strcpy(outputs[0], "Opening \"help\".");
		if (strcmp(inputs[0], "quit") == 0)
			strcpy(outputs[0], "Opening \"home\".");
		if (strcmp(inputs[0], "config") == 0)
			strcpy(outputs[0], "Opening \"config\".");

	}
}
