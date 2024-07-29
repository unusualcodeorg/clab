#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

/*
0->1->2->3
4->5->6->7
.........
 */
Graph2DMap *util_graph_from_2d_arr(char ***arr, int rows, int cols, bool autofree) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create(autofree);
  HashMap *idmap = hashmap_create(rows * cols, true);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char key[50];
      char *data = arr[i][j];
      snprintf(key, 50, "%s", data);

      int id = i * cols + j;
      int upid = id - cols;
      int backid = j > 0 ? id - 1 : -1;

      int *mid = malloc(sizeof(int));
      if (upid < 0 && backid < 0)  // can not link to any
      {
        int nid = graph_insert(graph, data, 0);
        *mid = nid;
      } else if (upid >= 0 && backid < 0)  // can link up only
      {
        int nid = graph_insert(graph, data, 1, (unsigned int)upid);
        *mid = nid;
      } else if (upid < 0 && backid >= 0)  // can link back only
      {
        int nid = graph_insert(graph, data, 1, (unsigned int)backid);
        *mid = nid;
      } else  // can link up and back
      {
        int nid = graph_insert(graph, data, 2, (unsigned int)upid, (unsigned int)backid);
        *mid = nid;
      }

      hashmap_put(idmap, key, mid);
    }
  }

  Graph2DMap *gmap = malloc(sizeof(Graph2DMap));
  gmap->graph = graph;
  gmap->idmap = idmap;
  return gmap;
}

char ***util_create_2d_str_arr(int rows, int cols) {
  char ***arr = malloc(rows * sizeof(char **));
  for (int i = 0; i < rows; i++) {
    arr[i] = malloc(cols * sizeof(char *));
    for (int j = 0; j < cols; j++) arr[i][j] = malloc(50 * sizeof(char));
  }
  return arr;
}

void util_destroy_2d_str_arr(char ***arr, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      free(arr[i][j]);
    }
    free(arr[i]);
  }
  free(arr);
}
