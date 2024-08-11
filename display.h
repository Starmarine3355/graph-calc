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
	NORMAL,
	HELP,
	HISTORY
};

/* draws a horizontal line
 */
void print_line();

/* saves axis lines to plot.
 */
void draw_axis(struct graph *p_plot);

/* saves a ' ' to all plot indexes, thus erasing it.
 */
void reset_plot(struct graph *p_plot);

/* displays normal- (plot, last 5 IOs, prompt), help- (help message, prompt) or history mode (all IOs, prompt)
 * gets input from the user.
 * new input and output is saved to inputs/outputs in the first index and old elements are shifted one index.
 *
 * returns 1 if input is longer than MAX_BUF, else 0
 */
struct result display(enum display_mode mode, struct graph plot, char inputs[MAX_IO_LINES][MAX_BUF], char outputs[MAX_IO_LINES][MAX_BUF]);

#endif
