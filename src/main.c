#include "crun/demo.h"
#include "dslib/demo.h"
#include "crun/demo.h"
#include "term/demo.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum COMMAND
{
	NONE,
	ALL,
	STACK,
	QUEUE,
	RUNTIME,
	GRAPH,
	TREE,
	HASHMAP,
	CONSOLE,
	PATH,
};

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "%s  - one command needed [all, stack, queue, runtime, graph, tree, hashmap, console, path]\n", argv[0]);
		return EXIT_FAILURE;
	}

	int option = NONE;
	if (strcmp(argv[1], "all") == 0)
		option = ALL;
	if (strcmp(argv[1], "stack") == 0)
		option = STACK;
	else if (strcmp(argv[1], "queue") == 0)
		option = QUEUE;
	else if (strcmp(argv[1], "runtime") == 0)
		option = RUNTIME;
	else if (strcmp(argv[1], "graph") == 0)
		option = GRAPH;
	else if (strcmp(argv[1], "tree") == 0)
		option = TREE;
	else if (strcmp(argv[1], "hashmap") == 0)
		option = HASHMAP;
	else if (strcmp(argv[1], "console") == 0)
		option = CONSOLE;
	else if (strcmp(argv[1], "path") == 0)
		option = PATH;

	switch (option)
	{
	case ALL:
		stack_demo();
		stack_concurrent_demo();
		queue_demo();
		queue_concurrent_demo();
		runtime_demo();
		runpool_demo();
		graph_demo();
		graph_concurrent_demo();
		graph_2d_arr_demo();
		tree_demo();
		hashmap_demo();
		path_shortest_nw_graph_demo();
		break;
	case STACK:
		stack_demo();
		stack_concurrent_demo();
		break;
	case QUEUE:
		queue_demo();
		queue_concurrent_demo();
		break;
	case RUNTIME:
		runtime_demo();
		runpool_demo();
		break;
	case GRAPH:
		graph_demo();
		graph_concurrent_demo();
		graph_2d_arr_demo();
		break;
	case TREE:
		tree_demo();
		break;
	case HASHMAP:
		hashmap_demo();
		break;
	case CONSOLE:
		console_demo();
		break;
	case PATH:
		path_shortest_nw_graph_demo();
		break;
	default:
		fprintf(stderr, "Unknown command name: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
