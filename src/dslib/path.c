#include "path.h"
#include "graph.h"
#include "tree.h"
#include "queue.h"
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

char *path_tree_data_to_string(void *arg)
{
	char data = *(char *)arg;
	char *buffer = malloc(50);
	snprintf(buffer, 50, "%c", data);
	return buffer;
}

/**
 * Queue help is traversing nebouring nodes before its linked nodes,
 * Stack holds the backtracked path,
 */
Stack *path_shortest_nw_graph(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid)
{
	Stack *stack = stack_create(false);

	GraphNode *start = graph_find(graph, srcnodeid);
	if (start == NULL)
		return stack;

	if (srcnodeid == dstnodeid)
	{
		stack_push(stack, start);
		return stack;
	}

	TreeNode *found = NULL;

	Tree *tree = tree_create(false);
	tree->debug = graph->debug;
	tree_add_root(tree, start);

	Queue *queue = queue_create(false);
	queue_enqueue(queue, tree->root);

	while (queue->size > 0)
	{
		TreeNode *tnode = queue_dequeue(queue);
		GraphNode *node = (GraphNode *)tnode->data;

		if (node->id == dstnodeid)
		{
			found = tnode;
			break;
		}

		for (unsigned short i = 0; i < node->esize; i++)
		{
			GraphEdge *edge = node->edges[i];
			if (edge == NULL || edge->end == NULL)
				continue;

			if (edge->end == node)
				continue;

			TreeNode *cnode = tree_add_node(tree, edge->end, tnode);
			queue_enqueue(queue, cnode);
		}
	}

	while (found != NULL)
	{
		stack_push(stack, found);
		found = found->parent;
	}

	if (graph->debug)
		tree_print(tree, path_tree_data_to_string);

	tree_destroy(tree);
	queue_destroy(queue);

	return stack;
}
