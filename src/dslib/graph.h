#include <stdbool.h>
#include <stdarg.h>

#ifndef DS_LIB_GRAPH_H
#define DS_LIB_GRAPH_H

#define GRAPH_NODE_NULL_ID -1;
#define GRAPH_ERROR -1;

#ifdef __cplusplus
extern "C"
{
#endif
	typedef char *(*DataToString)(void *);

	// Traversal direction will be opposite to the direction of visit
	typedef struct
	{
		unsigned short weight;
		struct GraphNode *end; // Use forward-declared GraphNode
	} GraphEdge;

	typedef struct GraphNode
	{
		unsigned int id;
		void *data;
		unsigned short esize;
		GraphEdge **edges;
	} GraphNode;

	/**
	 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
	 */
	typedef struct
	{
		bool debug;
		unsigned int size;
		bool autofree; // free data on pop
		GraphNode *root;
	} Graph;

	typedef void (*GraphTraversalCallback)(GraphNode *, void *arg);

	Graph *graph_create(bool autofree);
	GraphNode *graph_find(Graph *graph, unsigned int nodeid);
	void *graph_get(Graph *graph, unsigned int nodeid);
	int graph_add(Graph *graph, void *data, unsigned int linkcount, ...);
	int graph_remove(Graph *graph, unsigned int nodeid);
	void graph_print(Graph *graph, DataToString tostring);
	void graph_destroy(Graph *graph);

	// utils
	Graph *graph_from_2d_arr(char **arr, int rows, int cols, bool autofree);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_GRAPH_H
