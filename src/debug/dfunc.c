#include "dfunc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../demo/crundemo.h"
#include "../demo/dslibdemo.h"
#include "../demo/pathdemo.h"
#include "../demo/puzzledemo.h"
#include "../demo/termdemo.h"
#include "../dslib/hashmap.h"
#include "../puzzle/maze.h"

int debugfn(char *fname) {
  if (strlen(fname) == 0) {
    fprintf(stderr, "one of the debug function_name is needed \n");
    return EXIT_FAILURE;
  }

  HashMap *map = hashmap_create(50);
  hashmap_put(map, "stack_demo", (void *)stack_demo);
  hashmap_put(map, "stack_concurrent_demo", (void *)stack_concurrent_demo);
  hashmap_put(map, "queue_demo", (void *)queue_demo);
  hashmap_put(map, "queue_concurrent_demo", (void *)queue_concurrent_demo);
  hashmap_put(map, "runtime_demo", (void *)runtime_demo);
  hashmap_put(map, "runpool_demo", (void *)runpool_demo);
  hashmap_put(map, "tree_demo", (void *)tree_demo);
  hashmap_put(map, "graph_demo", (void *)graph_demo);
  hashmap_put(map, "graph_concurrent_demo", (void *)graph_concurrent_demo);
  hashmap_put(map, "graph_2d_arr_demo", (void *)graph_2d_arr_demo);
  hashmap_put(map, "hashmap_demo", (void *)hashmap_demo);
  hashmap_put(map, "path_shortest_nwg_tree_demo", (void *)path_shortest_nwg_tree_demo);
  hashmap_put(map, "console_demo", (void *)console_demo);
  hashmap_put(map, "maze_shortest_distance_demo", (void *)maze_shortest_distance_demo);
  hashmap_put(map, "maze_shortest_distance", (void *)maze_shortest_distance);
  hashmap_put(map, "linked_list_demo", (void *)linked_list_demo);
  hashmap_put(map, "path_shortest_demo", (void *)path_shortest_demo);
  hashmap_put(map, "path_permutation_demo", (void *)path_permutation_demo);
  hashmap_put(map, "path_permutation_buffered_demo", (void *)path_permutation_demo);
  hashmap_put(map, "path_permutation_pool_demo", (void *)path_permutation_pool_demo);
  hashmap_put(map, "path_permutation_pipeline_demo", (void *)path_permutation_pipeline_demo);
  hashmap_put(map, "maze_solution_demo", (void *)maze_solution_demo);
  hashmap_put(map, "maze_solution", (void *)maze_solution);

  ExceFunc func = (ExceFunc)hashmap_get(map, fname);

  if (!func) {
    fprintf(stderr, "%s not found \n", fname);
    return EXIT_FAILURE;
  }
  func();

  hashmap_destroy(map, NULL);
  return EXIT_SUCCESS;
}
