#include <stdlib.h>
#include <stdio.h>
#include "crun/demo.h"
#include "dslib/demo.h"
#include "crun/demo.h"

int main(void)
{
	stack_demo();
	stack_concurrent_demo();
	queue_demo();
	queue_concurrent_demo();
	crun_demo();
	return EXIT_SUCCESS;
}
