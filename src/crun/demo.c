#include "demo.h"
#include "runtime.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

/*--------------------RUNTIME DEMO------------------------ */

void work(void* context)
{
	(void)context; // to supress unused variable warning
	printf("Croutine: started\n");
	sleep(1);
	printf("Croutine: completed\n");
}

int crun_demo(void)
{
	printf("\n--------------RUNTIME DEMO----------------\n");
	Runtime *runtime = crun_create("Demo", true);

	for (int i = 0; i < 3; i++)
	{
		printf("work scheduled\n");
		crun_exec(runtime, work, NULL);
		sleep(2);
	}

	crun_destroy(runtime);
	printf("--------------RUNTIME DEMO----------------\n");
	return EXIT_SUCCESS;
}

/*--------------------RUNTIME DEMO------------------------ */
