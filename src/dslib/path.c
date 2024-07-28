#include "path.h"

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "queue.h"
#include "stack.h"
#include "tree.h"

char *graph_sd_data_to_string(void *arg) {
  char *data = (char *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%s", data);
  return buffer;
}

char *location_to_string(void *arg) {
  Location *location = (Location *)arg;
  char *data = (char *)location->data;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]%s", location->id, data);
  return buffer;
}

char *path_graph_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  char data = *(char *)node->data;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%c", data);
  return buffer;
}

char *path_tree_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]", node->id);
  return buffer;
}

Location *location_from_graph_node(GraphNode *node) {
  if (node == NULL) return NULL;
  Location *location = (Location *)malloc(sizeof(Location));
  location->id = node->id;
  location->data = node->data;
  return location;
}

Stack *path_shortest_nw_graph(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid) {
  return path_shortest_nw_graph_vis(graph, srcnodeid, dstnodeid, path_tree_data_to_string);
}

/**
 * Queue help is traversing nebouring nodes before its linked nodes,
 * Stack holds the backtracked path,
 * Traversal algo don't allow to go back and up
 */
Stack *path_shortest_nw_graph_vis(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid,
                                  DataToString tostring) {
  Stack *stack = stack_create(true);

  GraphNode *start = graph_find_bfs(graph, srcnodeid);
  if (start == NULL) return stack;

  if (srcnodeid == dstnodeid) {
    stack_push(stack, location_from_graph_node(start));
    return stack;
  }

  TreeNode *found = NULL;

  Tree *tree = tree_create(false);
  tree->debug = graph->debug;
  tree_insert_root(tree, start);

  Queue *queue = queue_create(false);
  queue_enqueue(queue, tree->root);

  while (queue->size > 0) {
    TreeNode *tnode = queue_dequeue(queue);
    GraphNode *node = (GraphNode *)tnode->data;

    if (node->id == dstnodeid) {
      found = tnode;
      break;
    }

    for (unsigned short i = 0; i < node->esize; i++) {
      GraphEdge *edge = node->edges[i];
      if (edge == NULL || edge->end == NULL) continue;

      if (tnode->parent != NULL) {
        GraphNode *back = (GraphNode *)tnode->parent->data;
        if (edge->end == back)  // don't go back
          continue;
      }

      TreeNode *cnode = tree_insert_node(tree, edge->end, tnode);
      queue_enqueue(queue, cnode);
    }
  }

  // backtrack from the destination to source
  while (found != NULL) {
    stack_push(stack, location_from_graph_node(found->data));
    found = found->parent;
  }

  if (graph->debug) tree_print_raw(tree, tostring);

  tree_destroy(tree);
  queue_destroy(queue);

  return stack;
}
