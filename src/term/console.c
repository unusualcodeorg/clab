#include "console.h"
#include "../crun/runtime.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void clear_console(void)
{
	// ANSI escape code to clear the screen
	printf("\033[H\033[J");
}

void set_cursor_position(int x, int y)
{
	// ANSI escape code to set cursor position
	printf("\033[%d;%dH", y, x);
}

void animate_text(const char *text, unsigned int delay)
{
	int x = 1; // Adjust the horizontal position as needed
	int y = 1; // Adjust the vertical position as needed

	clear_console();
	set_cursor_position(x, y);
	printf("%s\n", text);
	fflush(stdout); // Ensure the output is printed immediately
	usleep(delay);	// Delay for specified microseconds
	clear_console();
}

void render(void *context)
{
	(void)context;
	printf("Croutine: started\n");
	sleep(1);
	printf("Croutine: completed\n");
}

Console *console_create(unsigned int ftime_mills)
{
	Runtime *runtime = crun_create("Console", false);
	Console *console = (Console *)malloc(sizeof(Console));
	console->runtime = runtime;
	console->ftime = ftime_mills;
	return console;
}

void console_print(Console *console, char *text)
{
	(void)text;
	crun_exec(console->runtime, render, NULL);
}

void console_destroy(Console *console)
{
	crun_destroy(console->runtime);
	free(console);
}
