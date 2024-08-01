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
unsigned int cantor_pairing_uid(unsigned int x, unsigned int y) {
  return ((x + y) * (x + y + 1)) / 2 + y;
}

char *cantor_pairing_uid_str(unsigned int x, unsigned int y) {
  unsigned int uid = cantor_pairing_uid(x, y);
  char *key = (char *)malloc(50 * sizeof(char));
  snprintf(key, 50, "%d", uid);
  return key;
}

/*
0->1->2->3
4->5->6->7
.........
 */
Graph2DMap *util_graph_from_2d_arr(char ***arr, unsigned int rows, unsigned int cols) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create();
  HashMap *idmap = hashmap_create(rows * cols);

  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      char key[50];
      char *data = arr[i][j];
      snprintf(key, 50, "%s", data);
      Location *pathloc = location_create(data);

      int id = i * cols + j;
      int upid = id - cols;
      int backid = j > 0 ? id - 1 : -1;

      unsigned int *mid = malloc(sizeof(unsigned int));
      if (upid < 0 && backid < 0)  // can not link to any
      {
        unsigned int nid = graph_insert(graph, pathloc, 0);
        *mid = nid;
      } else if (upid >= 0 && backid < 0)  // can link up only
      {
        unsigned int nid = graph_insert(graph, pathloc, 1, (unsigned int)upid);
        *mid = nid;
      } else if (upid < 0 && backid >= 0)  // can link back only
      {
        unsigned int nid = graph_insert(graph, pathloc, 1, (unsigned int)backid);
        *mid = nid;
      } else  // can link up and back
      {
        unsigned int nid =
            graph_insert(graph, pathloc, 2, (unsigned int)upid, (unsigned int)backid);
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

char ***util_create_2d_str_arr(unsigned int rows, unsigned int cols, unsigned int capacity) {
  char ***arr = malloc(rows * sizeof(char **));
  for (unsigned int i = 0; i < rows; i++) {
    arr[i] = malloc(cols * sizeof(char *));
    for (unsigned int j = 0; j < cols; j++) {
      arr[i][j] = malloc(capacity * sizeof(char));
      snprintf(arr[i][j], capacity, "%d", 0);
    }
  }
  return arr;
}

void util_destroy_2d_str_arr(char ***arr, unsigned int rows, unsigned int cols) {
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      free(arr[i][j]);
    }
    free(arr[i]);
  }
  free(arr);
}
