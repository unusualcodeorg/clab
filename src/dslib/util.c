#include "util.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
0->1->2->3
4->5->6->7
.........
 */
Graph2DMap *util_graph_from_2d_arr(char **arr, int rows, int cols, bool autofree)
{
	if (arr == NULL)
		return NULL;

	Graph *graph = graph_create(autofree);
	HashMap *idmap = hashmap_create(rows * cols);
	char *key = malloc(50 * sizeof(char));

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			char *data = &arr[i][j];
			snprintf(key, 50, "%c", *data);

			int id = i * cols + j;
			int upid = id - cols;
			int backid = j > 0 ? id - 1 : -1;

			if (upid < 0 && backid < 0) // can not link to any
			{
				int nid = graph_insert(graph, data, 0);
				hashmap_put(idmap, key, nid);
			}
			else if (upid >= 0 && backid < 0) // can link up only
			{
				int nid = graph_insert(graph, data, 1, (unsigned int)upid);
				hashmap_put(idmap, key, nid);
			}
			else if (upid < 0 && backid >= 0) // can link back only
			{
				int nid = graph_insert(graph, data, 1, (unsigned int)backid);
				hashmap_put(idmap, key, nid);
			}
			else // can link up and back
			{
				int nid = graph_insert(graph, data, 2, (unsigned int)upid, (unsigned int)backid);
				hashmap_put(idmap, key, nid);
			}
		}
	}

	Graph2DMap *gmap = malloc(sizeof(Graph2DMap));
	gmap->graph = graph;
	gmap->idmap = idmap;
	free(key);
	return gmap;
}

void **util_create_2d_pt_arr(int rows, int cols, unsigned long elemsize)
{
	void **arr = malloc(rows * sizeof(void *));
	for (int i = 0; i < rows; i++)
		arr[i] = malloc(cols * elemsize);
	return arr;
}

void util_free_2d_pt_arr(void **arr, int rows)
{
	for (int i = 0; i < rows; i++)
		free(arr[i]);
	free(arr);
}
