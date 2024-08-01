#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"
#include "list.h"

#ifndef CLAB_DS_LIB_GRAPH_H
#define CLAB_DS_LIB_GRAPH_H

#define GRAPH_NODE_NULL_ID -1;
#define GRAPH_ERROR -1;

#ifdef __cplusplus
extern "C" {
#endif

// Traversal direction will be opposite to the direction of visit
typedef struct {
  unsigned short weight;
  struct GraphNode *end;  // Use forward-declared GraphNode
} GraphEdge;

typedef struct GraphNode {
  unsigned int id;
  void *data;
  unsigned short esize;
  GraphEdge **edges;
} GraphNode;

typedef struct {
  bool debug;
  unsigned int counter;
  pthread_rwlock_t rwlock;
  void *lambda;
} GraphCallbackArg;

typedef void (*GraphCallback)(GraphNode *, GraphCallbackArg *arg);
typedef void (*GraphDataCallback)(void *data);

/**
 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
 */
typedef struct {
  bool debug;
  unsigned int size;
  List *inodes;  // isolated nodes
  GraphNode *root;
  pthread_rwlock_t rwlock;
} Graph;

Graph *graph_create(void);
GraphNode *graph_find_bfs(Graph *graph, unsigned int nodeid);
GraphNode *graph_find_dfs(Graph *graph, unsigned int nodeid);
void *graph_get(Graph *graph, unsigned int nodeid);
// var arg for node ids to link with other nodes
int graph_insert(Graph *graph, void *data, unsigned int linkcount, ...);
int graph_insert_weighted(Graph *graph, void *data, unsigned int linkcount,
                          unsigned int *linknodeids, unsigned int *edgeweights);
int graph_delete(Graph *graph, unsigned int nodeid, FreeDataFunc freedatafunc);
void graph_print(Graph *graph, DataToString tostring);
void graph_traverse(Graph *graph, GraphDataCallback callback);
void graph_destroy(Graph *graph, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_GRAPH_H
