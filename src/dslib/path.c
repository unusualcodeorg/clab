#include "path.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "path.h"
#include "queue.h"
#include "stack.h"
#include "tree.h"

char *path_tree_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]", node->id);
  return buffer;
}

Location *location_create(void *data) {
  Location *location = (Location *)malloc(sizeof(Location));
  location->cost = UINT_MAX;
  location->id = 0;
  location->data = data;
  return location;
}

Location *location_copy(Location *loc) {
  Location *location = (Location *)malloc(sizeof(Location));
  location->cost = loc->cost;
  location->id = loc->id;
  location->data = loc->data;
  return location;
}

void location_cost_reset(void *data) {
  Location *loc = (Location *)data;
  loc->cost = UINT_MAX;
}

void free_location_data_func(void *data) {
  Location *loc = (Location *)data;
  free(loc->data);
  free(loc);
}

Stack *path_shortest_nwg_tree(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid) {
  return path_shortest_nwg_tree_vis(graph, srcnodeid, dstnodeid, path_tree_data_to_string);
}

/**
 * Queue help is traversing nebouring nodes before its linked nodes,
 * Stack holds the backtracked path,
 * Traversal algo don't allow to go back and up
 */
Stack *path_shortest_nwg_tree_vis(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid,
                                  DataToString tostring) {
  Stack *stack = stack_create();

  GraphNode *start = graph_find_bfs(graph, srcnodeid);
  if (start == NULL) return stack;

  GraphNode *dest = graph_find_bfs(graph, dstnodeid);
  if (dest == NULL) return stack;

  if (start == dest) {
    stack_push(stack, start->data);
    return stack;
  }

  TreeNode *found = NULL;

  Tree *tree = tree_create();
  tree->debug = graph->debug;
  tree_insert_root(tree, start);

  Queue *queue = queue_create();
  queue_enqueue(queue, tree->root);

  while (queue->size > 0) {
    TreeNode *tnode = queue_dequeue(queue, NULL);
    GraphNode *node = (GraphNode *)tnode->data;

    if (node == dest) {
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
    GraphNode *node = (GraphNode *)found->data;
    stack_push(stack, node->data);
    found = found->parent;
  }

  if (graph->debug) tree_print_raw(tree, tostring);
  if (graph->debug) printf("Tree Node: %d", tree->size);

  tree_destroy(tree, NULL);
  queue_destroy(queue, NULL);

  return stack;
}

/**
 * graph should contain data of type location
 */
Stack *path_find_shortest(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid) {
  Stack *stack = stack_create();  // store backtrack

  GraphNode *start = graph_find_bfs(graph, srcnodeid);
  if (start == NULL) return stack;

  GraphNode *dest = graph_find_bfs(graph, dstnodeid);
  if (dest == NULL) return stack;

  if (start == dest) {
    stack_push(stack, start->data);
    return stack;
  }

  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));

  ((Location *)start->data)->cost = 0;

  Queue *queue = queue_create();
  queue_enqueue(queue, start);

  // we have to visit all the nodes and update all the graph locations
  while (queue->size > 0) {
    GraphNode *node = queue_dequeue(queue, NULL);
    if (visited_nodes[node->id] == node) continue;
    visited_nodes[node->id] = node;

    if (node == dest) break;  // reached the destination

    Location *loc = (Location *)node->data;

    for (unsigned short i = 0; i < node->esize; i++) {
      GraphEdge *edge = node->edges[i];
      if (edge == NULL || edge->end == NULL) continue;

      unsigned int cost = loc->cost + edge->weight;

      Location *ezloc = (Location *)edge->end->data;
      if (ezloc->cost > cost) ezloc->cost = cost;

      queue_enqueue(queue, edge->end);
    }
  }

  // backtrack to find the shortest path
  GraphNode *current = dest;
  while (current != NULL) {
    Location *loc = (Location *)current->data;
    stack_push(stack, location_copy(loc));
    if (loc->cost == 0) break;

    GraphNode *next = NULL;
    unsigned int min = UINT_MAX;

    for (unsigned short i = 0; i < current->esize; i++) {
      GraphEdge *edge = current->edges[i];
      if (edge == NULL || edge->end == NULL) continue;

      Location *minloc = (Location *)edge->end->data;
      if (minloc->cost < min) {
        min = minloc->cost;
        next = edge->end;
      }
    }
    current = next;
  }

  // reset the location cost updated in the algo
  graph_traverse(graph, location_cost_reset);

  queue_destroy(queue, NULL);
  free(visited_nodes);
  return stack;
}
