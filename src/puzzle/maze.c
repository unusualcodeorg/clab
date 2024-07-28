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
  int idstore[rows][cols];

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
        idstore[i][j] = nid;
        continue;
      }

      int index = i * cols + j;
      int upindex = index - cols;
      int backindex = j > 0 ? index - 1 : -1;

      // skip trapped node
      if (upindex >= 0 && backindex >= 0) {
        int downindex = i == rows - 1 ? -1 : index + cols;
        int frontindex = j == cols - 1 ? -1 : index - 1;

        if (downindex >= 0 && frontindex >= 0) {
          if (strcmp(arr[i - 1][j], skip) == 0 && strcmp(arr[i][j - 1], skip) == 0 &&
              strcmp(arr[i + 1][j], skip) == 0 && strcmp(arr[i][j + 1], skip) == 0) {
            continue;
          }
        }
      }

      // this condition will never arrive, but adding here to cover all the cases
      if (upindex < 0 && backindex < 0) continue;

      int *mid = malloc(sizeof(int));

      if (upindex >= 0 && backindex < 0) {  // can link up only
        int up = idstore[i - 1][j];
        int nid = graph_insert_conditional(graph, data, true, 1, (unsigned int)up);
        *mid = nid;
      } else if (upindex < 0 && backindex >= 0) {  // can link back only
        int back = idstore[i][j - 1];
        int nid = graph_insert_conditional(graph, data, true, 1, (unsigned int)back);
        *mid = nid;
      } else {  // can link up and back
        int up = idstore[i - 1][j];
        int back = idstore[i][j - 1];
        int nid =
            graph_insert_conditional(graph, data, true, 2, (unsigned int)up, (unsigned int)back);
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
      char temp[10];
      char data = maze[i * cols + j];
      snprintf(temp, 10, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = maze_graph_map_create(arr, rows, cols, false);
  gmap->graph->debug = true;
  graph_print(gmap->graph, maze_data_to_string);

  unsigned int srcid = *(unsigned int *)hashmap_get(gmap->idmap, "S");
  unsigned int dstid = *(unsigned int *)hashmap_get(gmap->idmap, "G");

  Stack *stack = path_shortest_nw_graph_vis(gmap->graph, srcid, dstid, path_graph_data_to_string);

  graph_print(gmap->graph, graph_sd_data_to_string);
  stack_print(stack, location_to_string);

  stack_destroy(stack);
  graph_destroy(gmap->graph);
  hashmap_destroy(gmap->idmap);
  util_free_2d_str_arr(arr, rows, cols);
  return EXIT_SUCCESS;
}
