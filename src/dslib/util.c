#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
0->1->2->3
4->5->6->7
.........
 */
Graph *graph_from_2d_arr(char **arr, int rows, int cols, bool autofree)
{
	if (arr == NULL)
		return NULL;

	Graph *graph = graph_create(autofree);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			void *data = &arr[i][j];

			int id = i * cols + j;
			int upid = id - cols;
			int backid = id - 1;

			if (upid < 0 && backid < 0) // can not link to nay
				graph_add(graph, data, 0);
			else if (upid >= 0 && backid < 0) // can link up only
				graph_add(graph, data, 1, (unsigned int)upid);
			else if (upid < 0 && backid >= 0) // can link back only
				graph_add(graph, data, 1, (unsigned int)backid);
			else // can link up and back
				graph_add(graph, data, 2, (unsigned int)upid, (unsigned int)backid);
		}
	}

	return graph;
}
