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

unsigned int cantor_pairing_uid(unsigned int x, unsigned int y);
char *cantor_pairing_uid_str(unsigned int x, unsigned int y);
char ***util_create_2d_str_arr(unsigned int rows, unsigned int cols, unsigned int elemstrlen);
void util_destroy_2d_str_arr(char ***arr, unsigned int rows, unsigned int cols);

// char ***arr allows to hold variable length string elements
Graph2DMap *util_graph_from_2d_arr(char ***arr, unsigned int rows, unsigned int cols);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_UTIL_H
