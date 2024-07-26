#include <stdbool.h>
#include <stdarg.h>
#include <pthread.h>

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

	typedef struct
	{
		bool debug;
		unsigned int counter;
		void *lambda;
	} GraphCallbackArg;

	typedef void (*GraphCallback)(GraphNode *, GraphCallbackArg *arg);

	/**
	 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
	 */
	typedef struct
	{
		bool debug;
		bool autofree; // free data on pop
		unsigned int size;
		GraphNode *root;
		pthread_mutexattr_t mutexattr;
		pthread_mutex_t mutex;
	} Graph;

	Graph *graph_create(bool autofree);
	GraphNode *graph_find(Graph *graph, unsigned int nodeid);
	void *graph_get(Graph *graph, unsigned int nodeid);
	int graph_add(Graph *graph, void *data, unsigned int linkcount, ...); // var arg for node ids to link with
	int graph_remove(Graph *graph, unsigned int nodeid);
	void graph_print(Graph *graph, DataToString tostring);
	void graph_destroy(Graph *graph);

	// utility functions : graphutil.c
	Graph *graph_from_2d_arr(char **arr, int rows, int cols, bool autofree);

	// algorithms functions : graphalgo.c
	

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_GRAPH_H
