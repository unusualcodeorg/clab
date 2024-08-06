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

size_t cantor_pairing_uid(size_t x, size_t y);
char *cantor_pairing_uid_str(size_t x, size_t y);
char ***util_create_2d_str_arr(size_t rows, size_t cols, size_t elemstrlen);
void util_destroy_2d_str_arr(char ***arr, size_t rows, size_t cols);

// char ***arr allows to hold variable length string elements
Graph2DMap *util_graph_from_2d_arr(char ***arr, size_t rows, size_t cols);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_UTIL_H
