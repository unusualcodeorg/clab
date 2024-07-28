#include "maze.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../crun/runtime.h"
#include "../dslib/graph.h"
#include "../dslib/hashmap.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "../term/console.h"

char *maze_data_to_string(void *arg) {
  char data = *(char *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%c", data);
  return buffer;
}

/*
#########
#..@.#.@#
#@....G.#
#.#..@.@#
#.##@####
#..@.S..#
#########
*/
Graph2DMap *maze_graph_map_create(char ***arr, int rows, int cols, bool autofree) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create(autofree);
  HashMap *idmap = hashmap_create(rows * cols, true);

  char *skip = "#";

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char key[50];
      char *data = arr[i][j];
      snprintf(key, 50, "%s", data);

      // skip adding '#'
      if (strcmp(data, skip) == 0) continue;

      // add root logic and skip below
      if (graph->size == 0) {
        int *mid = malloc(sizeof(int));
        int nid = graph_insert(graph, data, 0);
        *mid = nid;
        hashmap_put(idmap, key, mid);
        continue;
      }

      int id = i * cols + j;
      int upid = id - cols;
      int backid = j > 0 ? id - 1 : -1;

      // skip trapped node
      if (upid >= 0 && backid >= 0) {
        int downid = id + cols;

        int fontid = j == cols - 1 ? -1 : id - 1;
        if (strcmp(arr[upid][j], skip) == 0 && strcmp(arr[i][backid], skip) == 0 &&
            strcmp(arr[downid][j], skip) == 0 && strcmp(arr[i][fontid], skip) == 0) {
        }
      }

      if (upid < 0 && backid < 0)  // this condition will never arrive, but
                                   // adding here to cover all the cases
        continue;

      int *mid = malloc(sizeof(int));

      // can link up only
      if (upid >= 0 && backid < 0) {
        int nid = graph_insert_conditional(graph, data, true, 1, (unsigned int)upid);
        *mid = nid;
      } else if (upid < 0 && backid >= 0)  // can link back only
      {
        int nid = graph_insert_conditional(graph, data, true, 1, (unsigned int)backid);
        *mid = nid;
      } else  // can link up and back
      {
        int nid = graph_insert_conditional(graph, data, true, 2, (unsigned int)upid,
                                           (unsigned int)backid);
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

int maze_shortest_distance(void) {
  const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";

  int rows = 7;
  int cols = 9;
  char ***arr = util_create_2d_str_arr(rows, cols);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char temp[50];
      char data = maze[i * cols + j];
      snprintf(temp, 50, "%d", data);
      strcpy(arr[i][j], temp);
    }
    printf("\n");
  }

  Graph2DMap *gmap =
      util_graph_from_2d_arr(arr, rows, cols, false);  // cannot auto free arr[i][j] since arr[i]
                                                       // is a continous memory
  gmap->graph->debug = true;
  graph_print(gmap->graph, maze_data_to_string);

  char *data = (char *)graph_get(gmap->graph, 40);
  printf("Graph found id %d : %c\n", 40, *data);

  data = (char *)graph_get(gmap->graph, 24);
  printf("Graph found id %d : %c\n", 24, *data);

  graph_destroy(gmap->graph);
  hashmap_destroy(gmap->idmap);
  util_free_2d_str_arr(arr, rows, cols);

  return EXIT_SUCCESS;
}
