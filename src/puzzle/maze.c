#include "maze.h"
#include "../dslib/graph.h"
#include "../dslib/util.h"
#include "../dslib/path.h"
#include "../dslib/hashmap.h"
#include "../crun/runtime.h"
#include "../term/console.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

char *maze_data_to_string(void *arg)
{
	char data = *(char *)arg;
	char *buffer = malloc(50);
	snprintf(buffer, 50, "%c", data);
	return buffer;
}

int maze_shortest_distance(void)
{
	const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";

	int rows = 7;
	int cols = 9;
	char ***arr = util_create_2d_str_arr(rows, cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			char temp[50];
			char data = maze[i * cols + j];
			snprintf(temp, 50, "%d", data);
			strcpy(arr[i][j], temp);
		}
		printf("\n");
	}

	Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols, false); // cannot auto free arr[i][j] since arr[i] is a continous memory
	gmap->graph->debug = true;
	graph_print(gmap->graph, maze_data_to_string);

	char *data = (char *)graph_get(gmap->graph, 40);
	printf("Graph found id %d : %c\n", 40, *data);

	data = (char *)graph_get(gmap->graph, 24);
	printf("Graph found id %d : %c\n", 24, *data);

	graph_destroy(gmap->graph);
	hashmap_destroy(gmap->idmap);
	util_free_2d_str_arr(arr, rows, cols);

	return EXIT_SUCCESS;
}
