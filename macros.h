#ifndef MACROS_H
#define MACROS_H

#define HEIGHT 51
#define WIDTH 101
#define MAX_BUF 91
#define MAX_IO_LINES 5

struct result 
{
	int code;
	char msg[MAX_BUF];
};

static inline struct result make_res(int code, char *msg)
{
	int i;
	struct result res;

	res.code = code;
	for (i = 0; i < MAX_BUF && msg[i] != '\0'; i++)
		res.msg[i] = msg[i]; 
	res.msg[i] = '\0';

	return res;
}

#endif
