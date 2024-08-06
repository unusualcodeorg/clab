#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "path.h"

/**
 *  pair(x, y) = (1/2) (x + y)(x + y + 1) + y
 *  gives uid using two integers
 */
size_t cantor_pairing_uid(size_t x, size_t y) {
  return ((x + y) * (x + y + 1)) / 2 + y;
}

char *cantor_pairing_uid_str(size_t x, size_t y) {
  size_t uid = cantor_pairing_uid(x, y);
  char *key = (char *)malloc(50 * sizeof(char));
  snprintf(key, 50, "%zu", uid);
  return key;
}

/*
0->1->2->3
4->5->6->7
.........
 */
Graph2DMap *util_graph_from_2d_arr(char ***arr, size_t rows, size_t cols) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create();
  HashMap *idmap = hashmap_create(rows * cols);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      char key[50];
      char *data = arr[i][j];
      snprintf(key, 50, "%s", data);
      Location *pathloc = location_create(data);

      int id = i * cols + j;
      int upid = id - cols;
      int backid = j > 0 ? id - 1 : -1;

      size_t *mid = malloc(sizeof(size_t));
      if (upid < 0 && backid < 0)  // can not link to any
      {
        size_t nid = graph_insert(graph, pathloc, 0);
        *mid = nid;
      } else if (upid >= 0 && backid < 0)  // can link up only
      {
        size_t nid = graph_insert(graph, pathloc, 1, (size_t)upid);
        *mid = nid;
      } else if (upid < 0 && backid >= 0)  // can link back only
      {
        size_t nid = graph_insert(graph, pathloc, 1, (size_t)backid);
        *mid = nid;
      } else  // can link up and back
      {
        size_t nid =
            graph_insert(graph, pathloc, 2, (size_t)upid, (size_t)backid);
        *mid = nid;
      }

      pathloc->id = *mid;
      hashmap_put(idmap, key, mid);
    }
  }

  Graph2DMap *gmap = malloc(sizeof(Graph2DMap));
  gmap->graph = graph;
  gmap->idmap = idmap;
  return gmap;
}

char ***util_create_2d_str_arr(size_t rows, size_t cols, size_t capacity) {
  char ***arr = malloc(rows * sizeof(char **));
  for (size_t i = 0; i < rows; i++) {
    arr[i] = malloc(cols * sizeof(char *));
    for (size_t j = 0; j < cols; j++) {
      arr[i][j] = malloc(capacity * sizeof(char));
      snprintf(arr[i][j], capacity, "%d", 0);
    }
  }
  return arr;
}

void util_destroy_2d_str_arr(char ***arr, size_t rows, size_t cols) {
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      free(arr[i][j]);
    }
    free(arr[i]);
  }
  free(arr);
}
