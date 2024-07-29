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
  void *data;
} Location;

Stack *path_shortest_nw_graph_vis(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid,
                                  DataToString tostring);  // non weighted graph
Stack *path_shortest_nw_graph(Graph *graph, unsigned int srcnodeid,
                              unsigned int dstnodeid);  // non weighted graph

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_PATH_H
