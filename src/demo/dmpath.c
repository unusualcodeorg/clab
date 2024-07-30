#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../crun/runtime.h"
#include "../dslib/datastr.h"
#include "../dslib/graph.h"
#include "../dslib/hashmap.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "../term/console.h"
#include "dmdslib.h"
#include "model.h"

/*-----------------------GRAPH 2D ARR DEMO-------------- */

int graph_2d_arr_demo(void) {
  printf("\n---------------GRAPH 2D ARR DEMO----------------\n");
  const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";

  int rows = 7;
  int cols = 9;
  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char temp[50];
      char data = maze[i * cols + j];
      snprintf(temp, 50, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols);
  gmap->graph->debug = true;
  graph_print(gmap->graph, char_data_to_string);

  char *data = (char *)graph_get(gmap->graph, 40);
  printf("Graph found id %d : %c\n", 40, *data);

  data = (char *)graph_get(gmap->graph, 24);
  printf("Graph found id %d : %c\n", 24, *data);

  graph_destroy(gmap->graph, NULL);  // data will be free using util
  hashmap_destroy(gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(arr, rows, cols);

  printf("\n---------------GRAPH 2D ARR DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*-----------------------GRAPH 2D ARR DEMO-------------- */

/*-------SHORTEST PATH NON WEIGHTED GRAPH DEMO--------- */

/*
 [0]A-----[1]B-----[2]C-----[3]D-----[4]E
    |        |        |        |        |
    |        |        |        |        |
 [5]F-----[6]G-----[7]H-----[8]I-----[9]J
    |        |        |        |        |
    |        |        |        |        |
 10]K----[11]L----[12]M----[13]N----[14]O
    |        |        |        |        |
    |        |        |        |        |
[15]P----[16]Q----[17]R----[18]S----[19]T
    |        |        |        |        |
    |        |        |        |        |
[20]U----[21]V----[22]W----[23]X----[24]Y
 */

void path_shortest_console(void *arg) {
  Runtime *rnc = (Runtime *)arg;
  Console *console = console_create(100);
  clock_t start = clock();
  double cputime = 0;

  while (rnc->exit != true) {
    clock_t end = clock();
    cputime = ((double)(end - start)) / CLOCKS_PER_SEC;

    char *text = malloc(50 * sizeof(char));
    snprintf(text, 50, "processing %f sec", cputime);
    console_render(console, text);
    usleep(1000000);
  }

  console_destroy(console);

  clock_t end = clock();
  cputime = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Execution time %f\n sec", cputime);
}

void path_shortest_nwg_tree_solution(void *arg) {
  Runtime *rnc = (Runtime *)arg;

  // int rows = 30;
  // int cols = 30;

  int rows = 5;
  int cols = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      // char str[50];
      // int num = i * cols + j;
      // snprintf(str, 50, "N%d", num);

      char str[2];
      char num = 65 + i * cols + j;
      snprintf(str, 2, "%c", num);

      strcpy(arr[i][j], str);
    }
  }

  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols);
  gmap->graph->debug = false;

  // unsigned int srcid = hashmap_get(gmap->idmap, "N6");
  // unsigned int dstid = hashmap_get(gmap->idmap, "N300");

  unsigned int srcid = *(unsigned int *)hashmap_get(gmap->idmap, "I");
  unsigned int dstid = *(unsigned int *)hashmap_get(gmap->idmap, "U");

  Stack *stack = path_shortest_nwg_tree_vis(gmap->graph, srcid, dstid,
                                            graph_node_char_data_to_string);  // G->S

  runtime_destroy(rnc);
  graph_print(gmap->graph, str_data_to_string);
  stack_print(stack, str_location_data_to_string);

  stack_destroy(stack, free_data_func);
  graph_destroy(gmap->graph, NULL);  // data will be free using util
  hashmap_destroy(gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(arr, rows, cols);
}

int path_shortest_nwg_tree_demo(void) {
  printf("\n-----SHORTEST PATH NON WEIGHTED GRAPH DEMO-----\n");

  Runtime *rns = runtime_create("Solution", false);
  Runtime *rnc = runtime_create("Console", false);

  runtime_exec(rns, path_shortest_nwg_tree_solution, rnc);
  runtime_exec(rnc, path_shortest_console, rnc);

  runtime_destroy(rns);

  printf("\n-----SHORTEST PATH NON WEIGHTED GRAPH DEMO-----\n");
  return EXIT_SUCCESS;
}

/*-------SHORTEST PATH NON WEIGHTED GRAPH DEMO--------- */
