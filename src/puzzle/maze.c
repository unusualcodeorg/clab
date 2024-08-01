#include "maze.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../crun/runtime.h"
#include "../dslib/datastr.h"
#include "../dslib/graph.h"
#include "../dslib/hashmap.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "../term/console.h"

void maze_sd_result_print(Stack *stack, char ***arr, unsigned int rows, unsigned int cols) {
  StackNode *node = stack->top;
  char ***patharr = util_create_2d_str_arr(rows, cols, 5);

  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      if (strcmp(arr[i][j], "#") != 0) {
        *patharr[i][j] = ' ';
        continue;
      }
      strcpy(patharr[i][j], arr[i][j]);
    }
  }

  while (node) {
    Location *loc = (Location *)node->data;
    unsigned int position = *(unsigned int *)loc->data;
    unsigned i = position / cols;
    unsigned j = position % cols;
    strcpy(patharr[i][j], arr[i][j]);
    node = node->next;
  }

  printf("\nShortest Path\n");
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      if (strcmp(patharr[i][j], "@") == 0) {
        printf("%c", '.');
        continue;
      }
      printf("%c", *patharr[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  util_destroy_2d_str_arr(patharr, rows, cols);
}

Graph2DMap *maze_graph_map_create(char ***arr, unsigned int rows, unsigned int cols, char *skip) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create();
  HashMap *idmap = hashmap_create(rows * cols);

  unsigned int idstore[rows][cols];
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      idstore[i][j] = rows * cols + 1;  // set max value which can not be asigned to any graph node
    }
  }

  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      unsigned int index = i * cols + j;

      unsigned int *data = malloc(sizeof(unsigned int));
      *data = index;
      Location *pathloc = location_create(data);

      char key[10];
      snprintf(key, 10, "%s", arr[i][j]);

      // skip adding '#'
      if (strcmp(key, skip) == 0) continue;

      // add root logic and skip below
      if (graph->size == 0) {
        unsigned int *mid = malloc(sizeof(unsigned int));
        unsigned int nid = graph_insert(graph, pathloc, 0);
        *mid = nid;
        hashmap_put(idmap, key, mid);
        idstore[i][j] = nid;
        continue;
      }

      long upindex = index - cols;
      long backindex = j == 0 ? -1 : index - 1;

      // skip trapped node
      if (upindex >= 0 && backindex >= 0) {
        long downindex = i == rows - 1 ? -1 : index + cols;
        long frontindex = j == cols - 1 ? -1 : index + 1;

        if (downindex >= 0 && frontindex >= 0) {
          if (strcmp(arr[i - 1][j], skip) == 0 && strcmp(arr[i][j - 1], skip) == 0 &&
              strcmp(arr[i + 1][j], skip) == 0 && strcmp(arr[i][j + 1], skip) == 0) {
            continue;
          }
        }
      }

      // this condition will never arrive, but adding here to cover all the cases
      if (upindex < 0 && backindex < 0) continue;

      unsigned int *mid = malloc(sizeof(unsigned int));
      unsigned int nid = 0;

      if (upindex >= 0 && backindex < 0) {  // can link up only
        unsigned int up = idstore[i - 1][j];
        nid = graph_insert(graph, pathloc, 1, up);
      } else if (upindex < 0 && backindex >= 0) {  // can link back only
        unsigned int back = idstore[i][j - 1];
        nid = graph_insert(graph, pathloc, 1, back);
      } else {  // can link up and back
        unsigned int up = idstore[i - 1][j];
        unsigned int back = idstore[i][j - 1];
        nid = graph_insert(graph, pathloc, 2, up, back);
      }

      idstore[i][j] = nid;
      pathloc->id = nid;
      *mid = nid;
      hashmap_put(idmap, key, mid);
    }
  }

  Graph2DMap *gmap = malloc(sizeof(Graph2DMap));
  gmap->graph = graph;
  gmap->idmap = idmap;
  return gmap;
}

void maze_find_shortest_distance(char ***arr, unsigned int rows, unsigned int cols, char *start,
                                 char *dest, char *skip) {
  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = maze_graph_map_create(arr, rows, cols, skip);
  gmap->graph->debug = true;

  unsigned int srcid = *(unsigned int *)hashmap_get(gmap->idmap, start);
  unsigned int dstid = *(unsigned int *)hashmap_get(gmap->idmap, dest);

  graph_print(gmap->graph, location_int_data_to_string);

  Stack *stack = path_find_shortest(gmap->graph, srcid, dstid);

  graph_print(gmap->graph, location_int_data_to_string);
  stack_print(stack, location_int_data_to_string);
  maze_sd_result_print(stack, arr, rows, cols);

  stack_destroy(stack, free_data_func);
  graph_destroy(gmap->graph, free_location_data_func);
  hashmap_destroy(gmap->idmap, free_data_func);
}

int maze_shortest_distance(void) {
  unsigned int rows, cols;
  unsigned int max = 1000;
  unsigned int elemstrlen = 5;

  printf("Enter number of rows: ");
  scanf("%d", &rows);

  printf("Enter number of columns: ");
  scanf("%d", &cols);

  if (rows > max || cols > max) {
    printf("Rows and Columns should less than %d", max);
    return EXIT_FAILURE;
  }

  printf("Enter elements of the %dx%d array:\n", rows, cols);
  char maze[rows][cols];
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      /**
       * space before %d tells scanf to ignore any leading whitespace characters
       * (including newlines)
       */
      scanf(" %c", &maze[i][j]);
    }
  }

  printf("\nYour input %dx%d array:\n", rows, cols);
  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      char buffer[elemstrlen];
      char data = maze[i][j];
      snprintf(buffer, elemstrlen, "%c", data);
      strcpy(arr[i][j], buffer);
      printf("%c", data);
    }
    printf("\n");
  }

  maze_find_shortest_distance(arr, rows, cols, "S", "G", "#");

  util_destroy_2d_str_arr(arr, rows, cols);
  return EXIT_SUCCESS;
}
