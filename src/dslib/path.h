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
  unsigned int id;
  unsigned int cost;
  void *data;
} Location;

Location *location_create(void *data);

void free_location_data_func(void *data);

char *path_tree_data_to_string(void *arg);

Stack *path_shortest_nwg_tree_vis(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid,
                                  DataToString tostring);  // non weighted graph
Stack *path_shortest_nwg_tree(Graph *graph, unsigned int srcnodeid,
                              unsigned int dstnodeid);  // non weighted graph

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_PATH_H
