#include "graph.h"
#include "stack.h"

#ifndef PATH_DEMO_H
#define PATH_DEMO_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		unsigned int id;
		void *data;
	} Location;

	Stack *path_shortest_nw_graph(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid); // non weighted graph

#ifdef __cplusplus
}
#endif

#endif // PATH_DEMO_H
