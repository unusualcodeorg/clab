#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdarg.h>

Graph *graph_create(bool autofree)
{
	Graph *graph = (Graph *)malloc(sizeof(Graph));
	graph->autofree = autofree;
	graph->debug = false;
	graph->root = NULL;
	graph->size = 1;
	pthread_rwlock_init(&graph->rwlock, NULL);
	return graph;
}

GraphNode *graph_node_find(GraphNode *node, unsigned int nodeid, GraphNode **visited_nodes, unsigned int *counter)
{
	*counter = *counter + 1;
	if (node == NULL || visited_nodes[node->id] == node)
		return NULL;

	visited_nodes[node->id] = node;

	if (node->id == nodeid)
		return node;

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;
		if (edge->end->id == nodeid)
			return edge->end;

		GraphNode *nd = graph_node_find(edge->end, nodeid, visited_nodes, counter);
		if (nd != NULL)
			return nd;
	}
	return NULL;
}

GraphNode *graph_find(Graph *graph, unsigned int nodeid)
{
	pthread_rwlock_rdlock(&graph->rwlock);
	if (graph->root == NULL)
		return NULL;

	unsigned int *counter = calloc(1, sizeof(unsigned int));
	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	GraphNode *node = graph_node_find(graph->root, nodeid, visited_nodes, counter);

	if (graph->debug == true)
		printf("\nGraph: Find Traversal = %u\n", *counter);

	free(visited_nodes);
	free(counter);
	pthread_rwlock_unlock(&graph->rwlock);
	return node;
}

void *graph_get(Graph *graph, unsigned int nodeid)
{
	pthread_rwlock_rdlock(&graph->rwlock);
	GraphNode *node = graph_find(graph, nodeid);
	if (node == NULL)
		return NULL;
	pthread_rwlock_unlock(&graph->rwlock);
	return node->data;
}

// rwlock is not enterant, causes deadlock if tries to acquire the lock again
int graph_add_root(Graph *graph, void *data)
{
	GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
	node->id = 0;
	node->data = data;
	node->esize = 0;
	node->edges = NULL;
	graph->root = node;
	return node->id;
}

int graph_add(Graph *graph, void *data, unsigned int linkcount, ...)
{
	pthread_rwlock_wrlock(&graph->rwlock);
	if (graph->root == NULL)
		return graph_add_root(graph, data);

	if (linkcount == 0)
		return GRAPH_ERROR;

	unsigned short nodeids[linkcount];
	va_list args;
	va_start(args, linkcount);

	for (unsigned int i = 0; i < linkcount; i++)
		nodeids[i] = va_arg(args, unsigned int);

	unsigned short esize = linkcount;

	GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
	node->id = graph->size++;
	node->data = data;
	node->esize = esize;
	node->edges = (GraphEdge **)calloc(esize, sizeof(GraphEdge *));

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphNode *n = graph_find(graph, nodeids[i]);
		if (n != NULL)
		{
			GraphEdge *edge = (GraphEdge *)malloc(sizeof(GraphEdge));
			edge->weight = 0;
			edge->end = n;
			node->edges[i] = edge;

			GraphEdge *ez = (GraphEdge *)malloc(sizeof(GraphEdge));
			ez->weight = 0;
			ez->end = node;
			n->esize++;
			n->edges = (GraphEdge **)realloc(n->edges, n->esize * sizeof(GraphEdge *));
			n->edges[n->esize - 1] = ez;
		}
	}

	va_end(args);
	pthread_rwlock_unlock(&graph->rwlock);
	return node->id;
}

int graph_remove(Graph *graph, unsigned int nodeid)
{
	pthread_rwlock_wrlock(&graph->rwlock);
	GraphNode *node = graph_find(graph, nodeid);
	if (node == NULL)
		return GRAPH_NODE_NULL_ID;

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;

		for (unsigned short j = 0; j < edge->end->esize; i++)
		{
			GraphEdge *ez = node->edges[j];
			if (ez == NULL || ez->end == NULL)
				continue;

			if (ez->end == node)
			{
				node->edges[j] = NULL;
				free(ez);
			}
		}

		free(edge);
	}

	free(node);
	pthread_rwlock_unlock(&graph->rwlock);
	return nodeid;
}

void traversal_callback(GraphNode *node, GraphCallbackArg *arg)
{
	(void)arg;
	bool debug = *(bool *)arg->p3;
	if (debug == true)
	{
		printf("[%u]->", node->id);
	}
}

void graph_traverse(GraphNode *node, GraphNode **visited_nodes, GraphCallback callback, GraphCallbackArg *arg)
{
	unsigned int *counter = (unsigned int *)arg->p1;
	*counter = *counter + 1;

	if (node == NULL || visited_nodes[node->id] == node)
		return;

	visited_nodes[node->id] = node;

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;
		graph_traverse(edge->end, visited_nodes, callback, arg);
	}

	if (callback != NULL)
		callback(node, arg);
}

void graph_print_node(GraphNode *node, GraphCallbackArg *arg)
{
	if (node == NULL)
		return;

	unsigned int *counter = (unsigned int *)arg->p1;
	*counter = *counter + 1;
	DataToString tostring = (DataToString)arg->p2;

	char *str = tostring(node->data);
	printf(" [%d]%s -->", node->id, str);
	free(str);

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;

		char *s = tostring(edge->end->data);
		printf(" [%d]%s", edge->end->id, s);
		free(s);
	}
	printf(",\n");
}

void graph_print(Graph *graph, DataToString tostring)
{
	pthread_rwlock_rdlock(&graph->rwlock);
	printf("\nGraph[\n");
	if (graph->root != NULL)
	{
		GraphCallbackArg *arg = (GraphCallbackArg *)malloc(sizeof(GraphCallbackArg));
		unsigned int *counter = (unsigned int *)calloc(1, sizeof(unsigned int));
		arg->p1 = counter;
		arg->p2 = (void *)tostring;
		arg->p3 = &graph->debug;

		GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
		graph_traverse(graph->root, visited_nodes, graph_print_node, arg);

		if (graph->debug == true)
			printf("\nGraph: Print Traversal = %u\n", *counter);

		free(visited_nodes);
		free(counter);
		free(arg);
	}
	printf("]\n");
	pthread_rwlock_unlock(&graph->rwlock);
}

void graph_node_destroy(GraphNode *node, bool autofree)
{
	if (node == NULL)
		return;

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge != NULL)
			free(edge);
	}
	free(node->edges);

	if (autofree)
		free(node->data);

	free(node);
}

void graph_destroy(Graph *graph)
{
	pthread_rwlock_wrlock(&graph->rwlock);

	if (graph->debug == true)
		printf("\n");

	GraphCallbackArg *arg = (GraphCallbackArg *)malloc(sizeof(GraphCallbackArg));
	unsigned int *counter = calloc(1, sizeof(unsigned int));
	arg->p1 = counter;
	arg->p3 = &graph->debug;

	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	graph_traverse(graph->root, visited_nodes, traversal_callback, arg);

	for (unsigned int i = 0; i < graph->size; i++)
	{
		GraphNode *node = visited_nodes[i];
		graph_node_destroy(node, graph->autofree);
	}

	if (graph->debug == true)
		printf("\nGraph: Destroy Traversal = %u\n", *counter);

	free(visited_nodes);
	free(counter);
	free(arg);
	pthread_rwlock_unlock(&graph->rwlock);
	pthread_rwlock_destroy(&graph->rwlock);
	free(graph);
}
