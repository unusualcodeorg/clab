#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

Graph *graph_create(bool autofree, bool debug)
{
	Graph *graph = (Graph *)malloc(sizeof(Graph));
	graph->autofree = autofree;
	graph->debug = debug;
	graph->root = NULL;
	graph->size = 0;
	return graph;
}

GraphNode *graph_node_find(GraphNode *node, uint64_t nodeid, GraphNode **visited_nodes)
{
	if (node == NULL || visited_nodes[nodeid] == node)
		return NULL;

	if (node->id == nodeid)
		return node;

	for (u_int8_t i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;
		if (edge->end->id == nodeid)
			return edge->end;

		GraphNode *nd = graph_node_find(edge->end, nodeid, visited_nodes);
		if (nd != NULL)
			return nd;
	}
	visited_nodes[node->id] = node;
	return NULL;
}

GraphNode *graph_find(Graph *graph, uint64_t nodeid)
{
	if (graph->root == NULL)
		return NULL;
	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	GraphNode *node = graph_node_find(graph->root, nodeid, visited_nodes);
	free(visited_nodes);
	return node;
}

void *graph_get(Graph *graph, uint64_t nodeid)
{
	GraphNode *node = graph_find(graph, nodeid);
	if (node == NULL)
		return NULL;

	return node->data;
}

int graph_add(Graph *graph, void *data, uint64_t nodeids[])
{
	if (graph->root != NULL && nodeids == NULL)
		return GRAPH_ERROR;

	u_int8_t esize = sizeof(*nodeids) / sizeof(uint64_t);

	if (graph->root != NULL && esize == 0)
		return GRAPH_ERROR;

	GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
	node->id = graph->size++;
	node->data = data;
	node->esize = esize;
	node->edges = (GraphEdge **)calloc(esize, sizeof(GraphEdge *));

	if (graph->root == NULL)
		graph->root = node;

	for (u_int8_t i = 0; i < node->esize; i++)
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

	return node->id;
}

int graph_remove(Graph *graph, uint64_t nodeid)
{
	GraphNode *node = graph_find(graph, nodeid);
	if (node == NULL)
		return GRAPH_NODE_NULL_ID;

	for (u_int8_t i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;

		for (u_int8_t j = 0; j < edge->end->esize; i++)
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
	return nodeid;
}

void graph_traverse(GraphNode *node, GraphNode **visited_nodes, GraphTraversalCallback lambda, void *arg)
{
	if (node == NULL || visited_nodes[node->id] == node)
		return;

	for (u_int8_t i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;
		graph_traverse(edge->end, visited_nodes, lambda, arg);
	}

	if (lambda != NULL)
		lambda(node, arg);

	visited_nodes[node->id] = node;
}

void graph_print_node(GraphNode *node, void *arg)
{
	if (node == NULL)
		return;

	DataToString tostring = (DataToString)arg;

	char *str = tostring(node->data);
	printf("{%" PRIu64 " [%s] --> ", node->id, str);
	free(str);

	for (u_int8_t i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;

		char *s = tostring(edge->end->data);
		printf("%" PRIu64 " [%s]", edge->end->id, s);
		free(s);

		if (i < node->esize)
			printf(",");
	}
	printf("}\n");
}

void graph_print(Graph *graph, DataToString tostring)
{
	printf("Graph[\n");
	if (graph->root != NULL)
	{
		GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
		graph_traverse(graph->root, visited_nodes, graph_print_node, (void *)tostring);
		free(visited_nodes);
	}
	printf("\n]\n");
}

void graph_node_destroy(GraphNode *node, void *arg)
{
	if (node == NULL)
		return;

	bool autofree = *(bool *)arg;

	for (u_int8_t i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[0];
		if (edge != NULL)
			free(edge);
	}
	free(node->edges);

	if (autofree == true)
		free(node->data);

	free(node);
}

void graph_destroy(Graph *graph)
{
	GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
	graph_traverse(graph->root, visited_nodes, graph_node_destroy, &graph->autofree);
	free(visited_nodes);
	free(graph);
}
