#include "graph.h"

// can lead to stackoverflow for large graphs
void graph_traverse_recursive(GraphNode *node, GraphNode **visited_nodes, GraphCallback callback, GraphCallbackArg *arg)
{
	if (arg->debug == true)
		arg->counter++;

	if (node == NULL || visited_nodes[node->id] == node)
		return;

	visited_nodes[node->id] = node;

	for (unsigned short i = 0; i < node->esize; i++)
	{
		GraphEdge *edge = node->edges[i];
		if (edge == NULL || edge->end == NULL)
			continue;
		graph_traverse_recursive(edge->end, visited_nodes, callback, arg);
	}

	if (arg->debug == true && callback != NULL)
		callback(node, arg);
}

GraphNode *graph_node_find_recursive(GraphNode *node, unsigned int nodeid, GraphNode **visited_nodes, GraphCallback callback, GraphCallbackArg *arg)
{
	if (arg->debug == true)
		arg->counter++;

	if (node == NULL || visited_nodes[node->id] == node)
		return NULL;

	if (arg->debug == true && callback != NULL)
		callback(node, arg);

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

		GraphNode *nd = graph_node_find_recursive(edge->end, nodeid, visited_nodes, callback, arg);
		if (nd != NULL)
			return nd;
	}
	return NULL;
}
