#include "graph.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "queue.h"
#include "stack.h"

Graph *graph_create(void) {
  Graph *graph = (Graph *)malloc(sizeof(Graph));
  graph->debug = false;
  graph->root = NULL;
  graph->size = 0;
  graph->inodes = list_create();
  pthread_rwlock_init(&graph->rwlock, NULL);
  return graph;
}

GraphCallbackArg *graph_default_callback_arg(Graph *graph) {
  GraphCallbackArg *arg = (GraphCallbackArg *)malloc(sizeof(GraphCallbackArg));
  arg->counter = 0;
  arg->debug = graph->debug;
  return arg;
}

void graph_traversal_callback(GraphNode *node, GraphCallbackArg *arg) {
  if (arg->debug == true) printf("[%zu]->", node->id);
}

GraphNode *graph_node_find_bfs(GraphNode *start, size_t nodeid, GraphNode **visited_nodes,
                               GraphCallback callback, GraphCallbackArg *arg) {
  GraphNode *found = NULL;

  Queue *queue = queue_create();
  queue_enqueue(queue, start);

  while (queue->size > 0) {
    if (arg->debug == true) arg->counter++;

    GraphNode *node = queue_dequeue(queue, NULL);
    if (visited_nodes[node->id] == node) continue;
    visited_nodes[node->id] = node;

    if (callback != NULL) callback(node, arg);

    if (node->id == nodeid) {
      found = node;
      break;
    }

    for (size_t i = 0; i < node->esize; i++) {
      GraphEdge *edge = node->edges[i];
      if (edge == NULL || edge->end == NULL) continue;

      if (edge->end->id == nodeid) {
        found = edge->end;
        break;
      }

      queue_enqueue(queue, edge->end);
    }

    if (found != NULL) break;
  }

  queue_destroy(queue, NULL);
  return found;
}

GraphNode *graph_node_find_dfs(GraphNode *start, size_t nodeid, GraphNode **visited_nodes,
                               GraphCallback callback, GraphCallbackArg *arg) {
  GraphNode *found = NULL;

  Stack *stack = stack_create();
  stack_push(stack, start);

  while (stack->size > 0) {
    if (arg->debug == true) arg->counter++;

    GraphNode *node = stack_pop(stack, NULL);
    if (visited_nodes[node->id] == node) continue;
    visited_nodes[node->id] = node;

    if (callback != NULL) callback(node, arg);

    if (node->id == nodeid) {
      found = node;
      break;
    }

    for (size_t i = 0; i < node->esize; i++) {
      GraphEdge *edge = node->edges[i];
      if (edge == NULL || edge->end == NULL) continue;

      if (edge->end->id == nodeid) {
        found = edge->end;
        break;
      }

      stack_push(stack, edge->end);
    }

    if (found != NULL) break;
  }

  stack_destroy(stack, NULL);
  return found;
}

GraphNode *graph_find_dfs(Graph *graph, size_t nodeid) {
  if (graph->root == NULL || nodeid > graph->size) return NULL;
  pthread_rwlock_rdlock(&graph->rwlock);

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
  GraphNode *node =
      graph_node_find_dfs(graph->root, nodeid, visited_nodes, graph_traversal_callback, arg);

  if (graph->debug == true) printf("\nGraph: Find DFS Traversal = %zu\n", arg->counter);

  free(visited_nodes);
  free(arg);
  pthread_rwlock_unlock(&graph->rwlock);
  return node;
}

GraphNode *graph_find_bfs(Graph *graph, size_t nodeid) {
  if (graph->root == NULL || nodeid > graph->size) return NULL;
  pthread_rwlock_rdlock(&graph->rwlock);

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
  GraphNode *node =
      graph_node_find_bfs(graph->root, nodeid, visited_nodes, graph_traversal_callback, arg);

  if (graph->debug == true) printf("\nGraph: Find BFS Traversal = %zu\n", arg->counter);

  free(visited_nodes);
  free(arg);
  pthread_rwlock_unlock(&graph->rwlock);
  return node;
}

void *graph_get(Graph *graph, size_t nodeid) {
  GraphNode *node = graph_find_bfs(graph, nodeid);
  return node != NULL ? node->data : NULL;
}

size_t graph_insert_root(Graph *graph, void *data) {
  if (graph->root != NULL) return GRAPH_NODE_NULL_ID;

  GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
  node->id = 1;
  node->data = data;
  node->esize = 0;
  node->edges = NULL;
  graph->root = node;
  graph->size = 1;
  return node->id;
}

bool graph_isolated_node_matcher(void *item, void *match) {
  GraphNode *node = (GraphNode *)item;
  size_t id = *(size_t *)match;
  return node->id == id;
}

size_t graph_insert(Graph *graph, void *data, size_t linkcount, ...) {
  va_list args;
  va_start(args, linkcount);

  size_t nodeids[linkcount];
  for (size_t i = 0; i < linkcount; i++) {
    nodeids[i] = va_arg(args, size_t);
  }

  size_t id = graph_insert_arr(graph, data, linkcount, nodeids);

  va_end(args);
  return id;
}

size_t graph_insert_arr(Graph *graph, void *data, size_t linkcount, size_t *nodeids) {
  pthread_rwlock_wrlock(&graph->rwlock);
  if (graph->debug == true) printf("\nGraph: Add To %zu Node\n", linkcount);

  if (graph->root == NULL) {
    size_t id = graph_insert_root(graph, data);
    pthread_rwlock_unlock(&graph->rwlock);
    return id;
  }

  if (linkcount == 0) {
    pthread_rwlock_unlock(&graph->rwlock);
    return GRAPH_NODE_NULL_ID;
  }

  GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
  node->id = ++graph->size;
  node->data = data;
  node->esize = 0;
  node->edges = NULL;

  GraphEdge *edges[linkcount];
  for (size_t i = 0; i < linkcount; i++) {
    edges[i] = NULL;
  }

  size_t edgecount = 0;
  bool isolated = true;
  for (size_t i = 0; i < linkcount; i++) {
    pthread_rwlock_unlock(&graph->rwlock);
    GraphNode *gnode = graph_find_bfs(graph, nodeids[i]);
    pthread_rwlock_wrlock(&graph->rwlock);

    if (gnode != NULL) {
      isolated = false;
    } else {
      // check in isolated nodes
      long index = list_index_of(graph->inodes, &nodeids[i], graph_isolated_node_matcher);
      if (index >= 0) gnode = list_get_at(graph->inodes, index);
    }

    if (gnode == NULL) continue;

    GraphEdge *edge = (GraphEdge *)malloc(sizeof(GraphEdge));
    edge->weight = 1;
    edge->end = gnode;
    edges[i] = edge;

    GraphEdge *ez = (GraphEdge *)malloc(sizeof(GraphEdge));
    ez->weight = 1;
    ez->end = node;
    gnode->esize++;
    gnode->edges = (GraphEdge **)realloc(gnode->edges, gnode->esize * sizeof(GraphEdge *));
    gnode->edges[gnode->esize - 1] = ez;
    edgecount++;
  }

  if (edgecount > 0) {
    node->edges = (GraphEdge **)malloc(edgecount * sizeof(GraphEdge *));
    for (size_t i = 0; i < linkcount; i++) {
      if (edges[i]) node->edges[node->esize++] = edges[i];
    }
  }

  if (isolated == true) list_add(graph->inodes, node);

  pthread_rwlock_unlock(&graph->rwlock);
  return node->id;
}

size_t graph_delete(Graph *graph, size_t nodeid, FreeDataFunc freedatafunc) {
  GraphNode *node = graph_find_dfs(graph, nodeid);
  if (node == NULL) {
    return GRAPH_NODE_NULL_ID;
  }

  pthread_rwlock_wrlock(&graph->rwlock);

  for (size_t i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge == NULL || edge->end == NULL) continue;

    for (size_t j = 0; j < edge->end->esize; j++) {
      GraphEdge *ez = node->edges[j];
      if (ez == NULL || ez->end == NULL) continue;

      if (ez->end == node) {
        node->edges[j] = NULL;  // TODO: resize the array
        free(ez);
      }
    }

    free(edge);
  }

  if (freedatafunc != NULL) free(node->data);
  free(node);
  pthread_rwlock_unlock(&graph->rwlock);
  return nodeid;
}

void graph_print_node(GraphNode *node, GraphCallbackArg *arg) {
  if (node == NULL) return;

  if (arg->debug == true) arg->counter++;

  char *str = arg->tostring(node->data);
  printf(" [%zd]%s -->", node->id, str);
  free(str);

  for (size_t i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge == NULL || edge->end == NULL) continue;

    char *s = arg->tostring(edge->end->data);
    printf(" [%zd]%s", edge->end->id, s);
    free(s);
  }
  printf(",\n");
}

void graph_traverse(Graph *graph, GraphDataCallback callback) {
  pthread_rwlock_rdlock(&graph->rwlock);
  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
  // 0 id means traverse the whole graph
  graph_node_find_dfs(graph->root, 0, visited_nodes, graph_traversal_callback, arg);

  // at i = 0 node is null since graph id starts with 1
  for (size_t i = 1; i <= graph->size; i++) {
    GraphNode *node = visited_nodes[i];
    callback(node->data);
    arg->counter++;
  }

  if (graph->debug == true) printf("\nGraph: Destroy DFS Traversal = %zu\n", arg->counter);

  free(visited_nodes);
  free(arg);
  pthread_rwlock_unlock(&graph->rwlock);
}

Graph *graph_clone(Graph *graph, GraphDataCopier datacopier) {
  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  // visited_nodes arr position is the node id
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
  // 0 id means traverse the whole graph
  graph_node_find_dfs(graph->root, 0, visited_nodes, NULL, arg);

  Graph *gcopy = graph_create();

  // at i = 0 node is null since graph id starts with 1
  for (size_t i = 1; i <= graph->size; i++) {
    GraphNode *node = visited_nodes[i];
    if (node == NULL) continue;

    void *datacopy = datacopier(node->data);

    if (i == 0) {
      graph_insert_root(gcopy, datacopy);
      continue;
    }

    size_t linknodeids[node->esize];
    for (size_t j = 0; j < node->esize; j++) {
      GraphEdge *edge = node->edges[j];
      linknodeids[j] = edge != NULL ? edge->end->id : SIZE_MAX;
    }

    graph_insert_arr(gcopy, datacopy, node->esize, linknodeids);
  }

  free(visited_nodes);
  free(arg);
  return gcopy;
}

void graph_print(Graph *graph, DataToString tostring) {
  pthread_rwlock_rdlock(&graph->rwlock);
  size_t counter = 0;
  printf("\nGraph[\n");
  if (graph->root != NULL) {
    GraphCallbackArg *arg = graph_default_callback_arg(graph);
    arg->tostring = tostring;

    GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
    // 0 id means traverse the whole graph
    graph_node_find_bfs(graph->root, 0, visited_nodes, graph_print_node, arg);
    counter = arg->counter;
    free(visited_nodes);
    free(arg);
  }
  printf("]\n");

  if (graph->debug == true) printf("Graph: Print BFS Traversal = %zu\n\n", counter);
  pthread_rwlock_unlock(&graph->rwlock);
}

void graph_node_destroy(GraphNode *node, FreeDataFunc freedatafunc) {
  if (node == NULL) return;

  for (size_t i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge != NULL) free(edge);
  }

  free(node->edges);
  if (freedatafunc != NULL) freedatafunc(node->data);
  free(node);
}

void graph_destroy(Graph *graph, FreeDataFunc freedatafunc) {
  pthread_rwlock_trywrlock(&graph->rwlock);  // thread will return if lock in not available

  if (graph->debug == true) printf("\n");

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size + 1, sizeof(GraphNode *));
  // 0 id means traverse the whole graph
  graph_node_find_dfs(graph->root, 0, visited_nodes, graph_traversal_callback, arg);

  // at i = 0 node is null since graph id starts with 1
  for (size_t i = 1; i <= graph->size; i++) {
    GraphNode *node = visited_nodes[i];

    long index = list_index_of(graph->inodes, &node->id, graph_isolated_node_matcher);
    if (index >= 0) list_delete_at(graph->inodes, index, NULL);

    graph_node_destroy(node, freedatafunc);
    arg->counter++;
  }

  list_destroy(graph->inodes, free_data_func);

  if (graph->debug == true) printf("\nGraph: Destroy DFS Traversal = %zu\n", arg->counter);

  free(visited_nodes);
  free(arg);
  pthread_rwlock_unlock(&graph->rwlock);
  pthread_rwlock_destroy(&graph->rwlock);
  free(graph);
}
