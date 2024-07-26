#ifndef DISPLAY_H
#define DISPLAY_H

#include "macros.h"

struct point
{
	float x, y;
};

struct graph
{
	char stream[HEIGHT][WIDTH];
	float scale; // y-axis : x-axis
	struct point pos; // camera position
};

enum display_mode
{
	NORMAL, // Graph + IOs
	HELP,	// Help message
	HISTORY	// History + Config
};

/* saves axis lines to plot.
 */
void draw_axis(struct graph *p_plot);

/* saves a ' ' to all plot indexes.
 */
void reset_plot(struct graph *p_plot);

/* prints graph, I/O box, and the command prompt.
 * gets input from the user.
 * new input and output is saved to inputs/outputs in the first index and old elements are shifted one index.
 *
 * returns 1 if input is longer than MAX_BUF, else 0
 */
struct result display(struct graph plot, char inputs[MAX_IO_LINES][MAX_BUF], char outputs[MAX_IO_LINES][MAX_BUF]);

#endif
