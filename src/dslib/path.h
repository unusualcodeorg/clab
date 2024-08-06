#include "datastr.h"
#include "graph.h"
#include "stack.h"

#ifndef CLAB_DS_LIB_PATH_H
#define CLAB_DS_LIB_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

// this is used to send the result of the path search, id is the graph id, and data is graph data
typedef struct {
  size_t id;
  size_t cost;
  void *data;
} Location;

Location *location_create(void *data);
Location *location_clone(Location *loc);

void free_location_data_func(void *data);

char *path_tree_data_to_string(void *arg);

// non weighted graph
Stack *path_shortest_nwg_tree_vis(Graph *graph, size_t srcnodeid, size_t dstnodeid,
                                  DataToString tostring);
// non weighted graph
Stack *path_shortest_nwg_tree(Graph *graph, size_t srcnodeid, size_t dstnodeid);

// any graph with search better algorithm
// graph must contain location data
Stack *path_find_shortest(Graph *graph, size_t srcnodeid, size_t dstnodeid);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_PATH_H
