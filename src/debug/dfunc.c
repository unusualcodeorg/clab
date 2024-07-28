#include "dfunc.h"
#include "../crun/demo.h"
#include "../dslib/demo.h"
#include "../crun/demo.h"
#include "../term/demo.h"
#include "../dslib/hashmap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int debugfn(char *fname)
{
	if (strlen(fname) == 0)
	{
		fprintf(stderr, "one of the debug function_name is needed \n");
		return EXIT_FAILURE;
	}

	HashMap *map = hashmap_create(50, false);
	hashmap_put(map, "stack_demo", (void *)stack_demo);
	hashmap_put(map, "stack_concurrent_demo", (void *)stack_concurrent_demo);
	hashmap_put(map, "queue_demo", (void *)queue_demo);
	hashmap_put(map, "queue_concurrent_demo", (void *)queue_concurrent_demo);
	hashmap_put(map, "runtime_demo", (void *)runtime_demo);
	hashmap_put(map, "runpool_demo", (void *)runpool_demo);
	hashmap_put(map, "graph_demo", (void *)graph_demo);
	hashmap_put(map, "graph_concurrent_demo", (void *)graph_concurrent_demo);
	hashmap_put(map, "graph_2d_arr_demo", (void *)graph_2d_arr_demo);
	hashmap_put(map, "hashmap_demo", (void *)hashmap_demo);
	hashmap_put(map, "path_shortest_nw_graph_demo", (void *)path_shortest_nw_graph_demo);
	hashmap_put(map, "console_demo", (void *)console_demo);

	debug_func_ptr_t func = (debug_func_ptr_t)hashmap_get(map, fname);

	if (!func)
	{
		fprintf(stderr, "%s not found \n", fname);
		return EXIT_FAILURE;
	}
	func();

	hashmap_destroy(map);
	return EXIT_SUCCESS;
}
