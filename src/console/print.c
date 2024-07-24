#include "print.h"
#include <stdio.h>
#include <unistd.h> // For usleep function

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

void animate_text(const char *texts[], int count)
{
	int x = 1;					 // Adjust the horizontal position as needed
	int y = 1;					 // Adjust the vertical position as needed
	int delay = 1000000; // 1 second delay

	for (int i = 0; i < count; i++)
	{
		clear_console();
		set_cursor_position(x, y);
		printf("%s\n", texts[i]);
		fflush(stdout); // Ensure the output is printed immediately
		usleep(delay);	// Delay for specified microseconds
	}

	clear_console();
	printf("Animation finished!\n");
}

int print_demo(void)
{
	const char *texts[] = {
			"First frame of text.",
			"Second frame of text.",
			"Third frame of text.",
			"Fourth frame of text."};
	int count = sizeof(texts) / sizeof(texts[0]);

	animate_text(texts, count);

	return 0;
}
