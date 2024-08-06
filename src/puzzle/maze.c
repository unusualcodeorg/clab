#include "maze.h"

#include <stdbool.h>
#include <stdint.h>
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

void maze_sd_result_print(Stack *stack, char ***arr, size_t rows, size_t cols) {
  StackNode *node = stack->top;
  char ***patharr = util_create_2d_str_arr(rows, cols, 5);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      if (strcmp(arr[i][j], "#") != 0) {
        *patharr[i][j] = ' ';
        continue;
      }
      strcpy(patharr[i][j], arr[i][j]);
    }
  }

  while (node) {
    Location *loc = (Location *)node->data;
    size_t position = *(size_t *)loc->data;
    size_t i = position / cols;
    size_t j = position % cols;
    strcpy(patharr[i][j], arr[i][j]);
    node = node->next;
  }

  printf("\nShortest Path\n");
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
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

Graph2DMap *maze_graph_map_create(char ***arr, size_t rows, size_t cols, char *skip) {
  if (arr == NULL) return NULL;

  Graph *graph = graph_create();
  HashMap *idmap = hashmap_create(rows * cols);

  size_t idstore[rows][cols];
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      idstore[i][j] = SIZE_MAX;  // set max value which can not be asigned to any graph node
    }
  }

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      size_t index = i * cols + j;

      size_t *data = malloc(sizeof(size_t));
      *data = index;
      Location *pathloc = location_create(data);

      char key[10];
      snprintf(key, 10, "%s", arr[i][j]);

      // skip adding '#'
      if (strcmp(key, skip) == 0) continue;

      // add root logic and skip below
      if (graph->size == 0) {
        size_t *mid = malloc(sizeof(size_t));
        size_t nid = graph_insert(graph, pathloc, 0);
        *mid = nid;
        hashmap_put(idmap, key, mid);
        idstore[i][j] = nid;
        continue;
      }

      long upindex = index - cols;
      long backindex = j == 0 ? -1 : (long)index - 1;

      // skip trapped node
      if (upindex >= 0 && backindex >= 0) {
        long downindex = i == rows - 1 ? -1 : (long)(index + cols);
        long frontindex = j == cols - 1 ? -1 : (long)index + 1;

        if (downindex >= 0 && frontindex >= 0) {
          if (strcmp(arr[i - 1][j], skip) == 0 && strcmp(arr[i][j - 1], skip) == 0 &&
              strcmp(arr[i + 1][j], skip) == 0 && strcmp(arr[i][j + 1], skip) == 0) {
            continue;
          }
        }
      }

      // this condition will never arrive, but adding here to cover all the cases
      if (upindex < 0 && backindex < 0) continue;

      size_t *mid = malloc(sizeof(size_t));
      size_t nid = 0;

      if (upindex >= 0 && backindex < 0) {  // can link up only
        size_t up = idstore[i - 1][j];
        nid = graph_insert(graph, pathloc, 1, up);
      } else if (upindex < 0 && backindex >= 0) {  // can link back only
        size_t back = idstore[i][j - 1];
        nid = graph_insert(graph, pathloc, 1, back);
      } else {  // can link up and back
        size_t up = idstore[i - 1][j];
        size_t back = idstore[i][j - 1];
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

void maze_find_shortest_distance(char ***arr, size_t rows, size_t cols, char *start, char *dest,
                                 char *skip) {
  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = maze_graph_map_create(arr, rows, cols, skip);
  gmap->graph->debug = true;

  size_t srcid = *(size_t *)hashmap_get(gmap->idmap, start);
  size_t dstid = *(size_t *)hashmap_get(gmap->idmap, dest);

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
  size_t rows, cols;
  size_t max = 1000;
  size_t elemstrlen = 5;

  printf("Enter number of rows: ");
  int scount = scanf("%zu", &rows);

  printf("Enter number of columns: ");
  scount = scanf("%zu", &cols);

  if (rows > max || cols > max) {
    printf("Rows and Columns should less than %zu", max);
    return EXIT_FAILURE;
  }

  printf("Enter elements of the %zux%zu array:\n", rows, cols);
  char maze[rows][cols];
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      /**
       * space before %d tells scanf to ignore any leading whitespace characters
       * (including newlines)
       */
      scount = scanf(" %c", &maze[i][j]);
    }
  }

  if (scount == 0) {
    perror("Error: Please check your input");
    exit(EXIT_FAILURE);
  }

  printf("\nYour input %zdx%zd array:\n", rows, cols);
  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
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
