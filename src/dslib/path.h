#include "graph.h"
#include "stack.h"

#ifndef PATH_DEMO_H
#define PATH_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char *(*DataToString)(void *);

// this is used to send the result of the path search, id is the graph id, and data is graph data
typedef struct {
  unsigned int id;
  void *data;
} Location;

Stack *path_shortest_nw_graph_vis(Graph *graph, unsigned int srcnodeid, unsigned int dstnodeid,
                                  DataToString tostring);  // non weighted graph
Stack *path_shortest_nw_graph(Graph *graph, unsigned int srcnodeid,
                              unsigned int dstnodeid);  // non weighted graph

char *graph_sd_data_to_string(void *arg);
char *location_to_string(void *arg);
char *path_graph_data_to_string(void *arg);

#ifdef __cplusplus
}
#endif

#endif  // PATH_DEMO_H
