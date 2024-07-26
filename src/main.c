#include <stdlib.h>
#include <stdio.h>
#include "crun/demo.h"
#include "dslib/demo.h"
#include "crun/demo.h"
#include "term/demo.h"

int main(void)
{
	stack_demo();
	stack_concurrent_demo();
	queue_demo();
	queue_concurrent_demo();
	runtime_demo();
	graph_demo();
	graph_concurrent_demo();
	graph_2d_arr_demo();
	tree_demo();
	console_demo();
	return EXIT_SUCCESS;
}
