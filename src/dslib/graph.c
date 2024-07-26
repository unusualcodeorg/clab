#include "graph.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdarg.h>

Graph *graph_create(bool autofree)
{
	Graph *graph = (Graph *)malloc(sizeof(Graph));
	graph->autofree = autofree;
	graph->debug = false;
	graph->root = NULL;
	graph->size = 0;
	pthread_mutexattr_init(&graph->mutexattr);
	pthread_mutexattr_settype(&graph->mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&graph->mutex, &graph->mutexattr);
	return graph;
}

GraphCallbackArg *graph_default_callback_arg(bool debug)
{
	GraphCallbackArg *arg = (GraphCallbackArg *)malloc(sizeof(GraphCallbackArg));
	arg->counter = 0;
	arg->debug = debug;
	return arg;
}

void graph_traversal_callback(GraphNode *node, GraphCallbackArg *arg)
{
	if (arg->debug == true)
		printf("[%u]->", node->id);
}

GraphNode *graph_node_find(GraphNode *start, unsigned int nodeid, GraphNode **visited_nodes, GraphCallback callback, GraphCallbackArg *arg)
{
	GraphNode *found = NULL;

	Stack *stack = stack_create(false);
	stack_push(stack, start);

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		GraphNode *node = stack_pop(stack);
		if (node == NULL || visited_nodes[node->id] == node)
			continue;
		visited_nodes[node->id] = node;

		if (arg->debug == true && callback != NULL)
			callback(node, arg);

		if (node->id == nodeid)
		{
			found = node;
			break;
		}

		for (unsigned short i = 0; i < node->esize; i++)
		{
			GraphEdge *edge = node->edges[i];
			if (edge == NULL || edge->end == NULL)
				continue;

			if (edge->end->id == nodeid)
			{
				found = edge->end;
				break;
			}

			stack_push(stack, edge->end);
		}

		if (found != NULL)
			break;
	}

	stack_destroy(stack);
	return found;
}

GraphNode *graph_find(Graph *graph, unsigned int nodeid)
{
	if (graph->root == NULL || nodeid >= graph->size)
		return NULL;

	pthread_mutex_lock(&graph->mutex);

	GraphCallbackArg *arg = graph_default_callback_arg(graph->debug);
	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	GraphNode *node = graph_node_find(graph->root, nodeid, visited_nodes, graph_traversal_callback, arg);

	if (graph->debug == true)
		printf("\nGraph: Find Traversal = %u\n", arg->counter);

	free(visited_nodes);
	free(arg);
	pthread_mutex_unlock(&graph->mutex);
	return node;
}

void *graph_get(Graph *graph, unsigned int nodeid)
{
	pthread_mutex_lock(&graph->mutex);
	GraphNode *node = graph_find(graph, nodeid);
	pthread_mutex_unlock(&graph->mutex);
	return node != NULL ? node->data : NULL;
}

// rwlock is not enterant, causes deadlock if tries to acquire the lock again
int graph_add_root(Graph *graph, void *data)
{
	if (graph->root != NULL)
		return GRAPH_ERROR;

	pthread_mutex_lock(&graph->mutex);
	GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
	node->id = 0;
	node->data = data;
	node->esize = 0;
	node->edges = NULL;
	graph->root = node;
	graph->size = 1;
	pthread_mutex_unlock(&graph->mutex);
	return node->id;
}

int graph_add(Graph *graph, void *data, unsigned int linkcount, ...)
{
	pthread_mutex_lock(&graph->mutex);
	if (graph->debug == true)
		printf("\nGraph: Add To %u Node\n", linkcount);

	if (graph->root == NULL)
	{
		unsigned int id = graph_add_root(graph, data);
		pthread_mutex_unlock(&graph->mutex);
		return id;
	}

	if (linkcount == 0)
	{
		pthread_mutex_unlock(&graph->mutex);
		return GRAPH_ERROR;
	}

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
	pthread_mutex_unlock(&graph->mutex);
	return node->id;
}

int graph_remove(Graph *graph, unsigned int nodeid)
{
	pthread_mutex_lock(&graph->mutex);
	GraphNode *node = graph_find(graph, nodeid);
	if (node == NULL)
	{
		pthread_mutex_unlock(&graph->mutex);
		return GRAPH_NODE_NULL_ID;
	}

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
				node->edges[j] = NULL; // TODO: resize the array
				free(ez);
			}
		}

		free(edge);
	}

	free(node);
	pthread_mutex_unlock(&graph->mutex);
	return nodeid;
}

void graph_traverse(GraphNode *start, GraphNode **visited_nodes, GraphCallback callback, GraphCallbackArg *arg)
{
	Stack *stack = stack_create(false);
	stack_push(stack, start);

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		GraphNode *node = stack_pop(stack);
		if (node == NULL || visited_nodes[node->id] == node)
			continue;
		visited_nodes[node->id] = node;

		for (unsigned short i = 0; i < node->esize; i++)
		{
			GraphEdge *edge = node->edges[i];
			if (edge == NULL || edge->end == NULL)
				continue;
			stack_push(stack, edge->end);
		}

		if (callback != NULL)
			callback(node, arg);
	}

	stack_destroy(stack);
}

void graph_print_node(GraphNode *node, GraphCallbackArg *arg)
{
	if (node == NULL)
		return;

	if (arg->debug == true)
		arg->counter++;

	DataToString tostring = (DataToString)arg->lambda;

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
	pthread_mutex_lock(&graph->mutex);
	unsigned int counter = 0;
	printf("\nGraph[\n");
	if (graph->root != NULL)
	{
		GraphCallbackArg *arg = graph_default_callback_arg(graph->debug);
		arg->lambda = (void *)tostring;

		GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
		graph_traverse(graph->root, visited_nodes, graph_print_node, arg);
		counter = arg->counter;
		free(visited_nodes);
		free(arg);
	}
	printf("]\n");

	if (graph->debug == true)
		printf("Graph: Print Traversal = %u\n\n", counter);
	pthread_mutex_unlock(&graph->mutex);
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
	pthread_mutex_lock(&graph->mutex);

	if (graph->debug == true)
		printf("\n");

	GraphCallbackArg *arg = graph_default_callback_arg(graph->debug);
	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	graph_traverse(graph->root, visited_nodes, graph_traversal_callback, arg);

	for (unsigned int i = 0; i < graph->size; i++)
	{
		GraphNode *node = visited_nodes[i];
		graph_node_destroy(node, graph->autofree);
	}

	if (graph->debug == true)
		printf("\nGraph: Destroy Traversal = %u\n", arg->counter);

	free(visited_nodes);
	free(arg);
	pthread_mutex_unlock(&graph->mutex);
	pthread_mutex_destroy(&graph->mutex);
	pthread_mutexattr_destroy(&graph->mutexattr);
	free(graph);
}
