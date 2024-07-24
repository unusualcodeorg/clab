#include "demo.h"
#include "runtime.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

/*--------------------RUNTIME DEMO------------------------ */

void work(void)
{
	printf("work start\n");
	sleep(2);
	printf("work completed\n");
}

int crun_demo(void)
{
	Runtime *runtime = crun_create("Demo Runtime", true);

	for (int i = 0; i < 3; i++)
	{
		printf("work scheduled\n");
		crun_exec(runtime, work);
		sleep(3);
	}

	crun_block(runtime);

	return EXIT_SUCCESS;
}

/*--------------------RUNTIME DEMO------------------------ */
