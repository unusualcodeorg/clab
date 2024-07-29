#include "graph.h"
#include "hashmap.h"

#ifndef CLAB_DS_LIB_UTIL_H
#define CLAB_DS_LIB_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  Graph *graph;
  HashMap *idmap;
} Graph2DMap;

char ***util_create_2d_str_arr(int rows, int cols);
void util_free_2d_str_arr(char ***arr, int rows, int cols);
Graph2DMap *util_graph_from_2d_arr(
    char ***arr, int rows, int cols,
    bool autofree);  // char ***arr allows to hold variable length string elements

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_UTIL_H
