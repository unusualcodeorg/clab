#include "graph.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
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
  arg->rwlock = graph->rwlock;
  return arg;
}

void graph_traversal_callback(GraphNode *node, GraphCallbackArg *arg) {
  if (arg->debug == true) printf("[%u]->", node->id);
}

GraphNode *graph_node_find_bfs(GraphNode *start, long nodeid, GraphNode **visited_nodes,
                               GraphCallback callback, GraphCallbackArg *arg) {
  pthread_rwlock_rdlock(&arg->rwlock);
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

    for (unsigned short i = 0; i < node->esize; i++) {
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
  pthread_rwlock_unlock(&arg->rwlock);
  return found;
}

GraphNode *graph_node_find_dfs(GraphNode *start, long nodeid, GraphNode **visited_nodes,
                               GraphCallback callback, GraphCallbackArg *arg) {
  pthread_rwlock_rdlock(&arg->rwlock);
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

    for (unsigned short i = 0; i < node->esize; i++) {
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
  pthread_rwlock_unlock(&arg->rwlock);
  return found;
}

GraphNode *graph_find_dfs(Graph *graph, unsigned int nodeid) {
  if (graph->root == NULL || nodeid >= graph->size) return NULL;

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
  GraphNode *node =
      graph_node_find_dfs(graph->root, nodeid, visited_nodes, graph_traversal_callback, arg);

  if (graph->debug == true) printf("\nGraph: Find DFS Traversal = %u\n", arg->counter);

  free(visited_nodes);
  free(arg);
  return node;
}

GraphNode *graph_find_bfs(Graph *graph, unsigned int nodeid) {
  if (graph->root == NULL || nodeid >= graph->size) return NULL;

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
  GraphNode *node =
      graph_node_find_bfs(graph->root, nodeid, visited_nodes, graph_traversal_callback, arg);

  if (graph->debug == true) printf("\nGraph: Find BFS Traversal = %u\n", arg->counter);

  free(visited_nodes);
  free(arg);
  return node;
}

void *graph_get(Graph *graph, unsigned int nodeid) {
  GraphNode *node = graph_find_bfs(graph, nodeid);
  return node != NULL ? node->data : NULL;
}

int graph_insert_root(Graph *graph, void *data) {
  if (graph->root != NULL) return GRAPH_ERROR;

  GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
  node->id = 0;
  node->data = data;
  node->esize = 0;
  node->edges = NULL;
  graph->root = node;
  graph->size = 1;
  return node->id;
}

bool graph_isolated_node_matcher(void *item, void *match) {
  GraphNode *node = (GraphNode *)item;
  unsigned int id = *(unsigned int *)match;
  return node->id == id;
}

int graph_insert(Graph *graph, void *data, unsigned int linkcount, ...) {
  pthread_rwlock_wrlock(&graph->rwlock);
  if (graph->debug == true) printf("\nGraph: Add To %u Node\n", linkcount);

  if (graph->root == NULL) {
    unsigned int id = graph_insert_root(graph, data);
    pthread_rwlock_unlock(&graph->rwlock);
    return id;
  }

  if (linkcount == 0) {
    pthread_rwlock_unlock(&graph->rwlock);
    return GRAPH_ERROR;
  }

  unsigned int nodeids[linkcount];
  va_list args;
  va_start(args, linkcount);

  for (unsigned int i = 0; i < linkcount; i++) {
    nodeids[i] = va_arg(args, unsigned int);
  }

  GraphNode *node = (GraphNode *)malloc(sizeof(GraphNode));
  node->id = graph->size++;
  node->data = data;
  node->esize = 0;
  node->edges = NULL;

  GraphEdge *edges[linkcount];
  for (unsigned int i = 0; i < linkcount; i++) {
    edges[i] = NULL;
  }

  unsigned int edgecount = 0;
  bool isolated = true;
  for (unsigned int i = 0; i < linkcount; i++) {
    GraphNode *gnode = graph_find_bfs(graph, nodeids[i]);
    if (gnode == NULL) {
      // check in isolated nodes
      unsigned int *nid = malloc(sizeof(unsigned int));
      *nid = nodeids[i];
      int index = list_index_of(graph->inodes, nid, graph_isolated_node_matcher);
      free(nid);
      if (index >= 0) {
        gnode = list_delete_at(graph->inodes, index, NULL);
      }
    } else {
      // if no node exits in the graph
      isolated = false;
    }

    if (gnode == NULL) continue;

    GraphEdge *edge = (GraphEdge *)malloc(sizeof(GraphEdge));
    edge->weight = 0;
    edge->end = gnode;
    edges[i] = edge;

    GraphEdge *ez = (GraphEdge *)malloc(sizeof(GraphEdge));
    ez->weight = 0;
    ez->end = node;
    gnode->esize++;
    gnode->edges = (GraphEdge **)realloc(gnode->edges, gnode->esize * sizeof(GraphEdge *));
    gnode->edges[gnode->esize - 1] = ez;

    edgecount++;
  }

  if (edgecount > 0) {
    node->edges = (GraphEdge **)malloc(edgecount * sizeof(GraphEdge *));
    for (unsigned int i = 0; i < linkcount; i++) {
      if (edges[i]) node->edges[node->esize++] = edges[i];
    }
  }

  if (isolated == true) list_add(graph->inodes, node);

  va_end(args);
  pthread_rwlock_unlock(&graph->rwlock);
  return node->id;
}

int graph_delete(Graph *graph, unsigned int nodeid, FreeDataFunc freedatafunc) {
  pthread_rwlock_wrlock(&graph->rwlock);
  GraphNode *node = graph_find_dfs(graph, nodeid);
  if (node == NULL) {
    pthread_rwlock_unlock(&graph->rwlock);
    return GRAPH_NODE_NULL_ID;
  }

  for (unsigned short i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge == NULL || edge->end == NULL) continue;

    for (unsigned short j = 0; j < edge->end->esize; j++) {
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

  DataToString tostring = (DataToString)arg->lambda;

  char *str = tostring(node->data);
  printf(" [%d]%s -->", node->id, str);
  free(str);

  for (unsigned short i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge == NULL || edge->end == NULL) continue;

    char *s = tostring(edge->end->data);
    printf(" [%d]%s", edge->end->id, s);
    free(s);
  }
  printf(",\n");
}

void graph_print(Graph *graph, DataToString tostring) {
  pthread_rwlock_rdlock(&graph->rwlock);
  unsigned int counter = 0;
  printf("\nGraph[\n");
  if (graph->root != NULL) {
    GraphCallbackArg *arg = graph_default_callback_arg(graph);
    arg->lambda = (void *)tostring;

    GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
    graph_node_find_bfs(graph->root, -1, visited_nodes, graph_print_node, arg);
    counter = arg->counter;
    free(visited_nodes);
    free(arg);
  }
  printf("]\n");

  if (graph->debug == true) printf("Graph: Print BFS Traversal = %u\n\n", counter);
  pthread_rwlock_unlock(&graph->rwlock);
}

void graph_node_destroy(GraphNode *node, bool autofree) {
  if (node == NULL) return;

  for (unsigned short i = 0; i < node->esize; i++) {
    GraphEdge *edge = node->edges[i];
    if (edge != NULL) free(edge);
  }
  free(node->edges);

  if (autofree) free(node->data);

  free(node);
}

void graph_destroy(Graph *graph, FreeDataFunc freedatafunc) {
  pthread_rwlock_trywrlock(&graph->rwlock);  // thread will return if lock in not available

  if (graph->debug == true) printf("\n");

  GraphCallbackArg *arg = graph_default_callback_arg(graph);
  GraphNode **visited_nodes = (GraphNode **)calloc(graph->size, sizeof(GraphNode *));
  graph_node_find_dfs(graph->root, -1, visited_nodes, graph_traversal_callback, arg);

  for (unsigned int i = 0; i < graph->size; i++) {
    GraphNode *node = visited_nodes[i];
    graph_node_destroy(node, freedatafunc);
    arg->counter++;
  }

  list_destroy(graph->inodes, free_data_func);

  if (graph->debug == true) printf("\nGraph: Destroy DFS Traversal = %u\n", arg->counter);

  free(visited_nodes);
  free(arg);
  pthread_rwlock_unlock(&graph->rwlock);
  pthread_rwlock_destroy(&graph->rwlock);
  free(graph);
}
