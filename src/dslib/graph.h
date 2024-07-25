#include <stdbool.h>
#include <stdint.h>

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
		int32_t weight;
		struct GraphNode *end; // Use forward-declared GraphNode
	} GraphEdge;

	typedef struct GraphNode
	{
		uint64_t id;
		void *data;
		u_int8_t esize;
		GraphEdge **edges;
	} GraphNode;

	/**
	 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
	 */
	typedef struct
	{
		bool debug;
		uint64_t size;
		bool autofree; // free data on pop
		GraphNode *root;
	} Graph;

	typedef void (*GraphTraversalCallback)(GraphNode *, void *arg);

	Graph *graph_create(bool autofree, bool debug);
	GraphNode *graph_find(Graph *graph, uint64_t nodeid);
	void *graph_get(Graph *graph, uint64_t nodeid);
	int graph_add(Graph *graph, void *data, uint64_t nodeids[]);
	int graph_remove(Graph *graph, uint64_t nodeid);
	void graph_print(Graph *graph, DataToString tostring);
	void graph_destroy(Graph *graph);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_GRAPH_H
