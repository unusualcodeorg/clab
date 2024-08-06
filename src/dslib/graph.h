#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"
#include "list.h"

#ifndef CLAB_DS_LIB_GRAPH_H
#define CLAB_DS_LIB_GRAPH_H

#define GRAPH_NODE_NULL_ID 0;

#ifdef __cplusplus
extern "C" {
#endif

// Traversal direction will be opposite to the direction of visit
typedef struct {
  size_t weight;
  struct GraphNode *end;  // Use forward-declared GraphNode
} GraphEdge;

typedef struct GraphNode {
  size_t id;
  void *data;
  size_t esize;
  GraphEdge **edges;
} GraphNode;

typedef struct {
  bool debug;
  size_t counter;
  pthread_rwlock_t *rwlock;
  DataToString tostring;
} GraphCallbackArg;

typedef void (*GraphCallback)(GraphNode *, GraphCallbackArg *arg);
typedef void (*GraphDataCallback)(void *data);
typedef void *(*GraphDataCopier)(void *data);

/**
 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
 */
typedef struct {
  bool debug;
  size_t size;
  List *inodes;  // isolated nodes
  GraphNode *root;
  pthread_rwlock_t rwlock;
} Graph;

Graph *graph_create(void);
GraphNode *graph_find_bfs(Graph *graph, size_t nodeid);
GraphNode *graph_find_dfs(Graph *graph, size_t nodeid);
void *graph_get(Graph *graph, size_t nodeid);
// var arg for node ids to link with other nodes
size_t graph_insert(Graph *graph, void *data, size_t linkcount, ...);
size_t graph_insert_arr(Graph *graph, void *data, size_t linkcount, size_t *nodeids);
size_t graph_delete(Graph *graph, size_t nodeid, FreeDataFunc freedatafunc);
void graph_print(Graph *graph, DataToString tostring);
void graph_traverse(Graph *graph, GraphDataCallback callback);
Graph *graph_clone(Graph *graph, GraphDataCopier datacopier);
void graph_destroy(Graph *graph, FreeDataFunc freedatafunc);

// size_t graph_insert_weighted(Graph *graph, void *data, size_t linkcount,
//                           size_t *linknodeids, size_t *edgeweights);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_GRAPH_H
