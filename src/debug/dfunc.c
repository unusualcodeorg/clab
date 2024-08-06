#include "dfunc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../demo/crundemo.h"
#include "../demo/dslibdemo.h"
#include "../demo/pathdemo.h"
#include "../demo/puzzledemo.h"
#include "../demo/termdemo.h"
#include "../puzzle/maze.h"

int debugfn(char *fname) {
  if (strlen(fname) == 0) {
    fprintf(stderr, "one of the debug function_name is needed \n");
    return EXIT_FAILURE;
  }

  ExceFunc func = NULL;

  if (strcmp(fname, "stack_demo") == 0)
    func = stack_demo;
  else if (strcmp(fname, "stack_concurrent_demo") == 0)
    func = stack_concurrent_demo;
  else if (strcmp(fname, "queue_demo") == 0)
    func = queue_demo;
  else if (strcmp(fname, "queue_concurrent_demo") == 0)
    func = queue_concurrent_demo;
  else if (strcmp(fname, "runtime_demo") == 0)
    func = runtime_demo;
  else if (strcmp(fname, "runpool_demo") == 0)
    func = runpool_demo;
  else if (strcmp(fname, "tree_demo") == 0)
    func = tree_demo;
  else if (strcmp(fname, "graph_demo") == 0)
    func = graph_demo;
  else if (strcmp(fname, "graph_concurrent_demo") == 0)
    func = graph_concurrent_demo;
  else if (strcmp(fname, "graph_2d_arr_demo") == 0)
    func = graph_2d_arr_demo;
  else if (strcmp(fname, "hashmap_demo") == 0)
    func = hashmap_demo;
  else if (strcmp(fname, "path_shortest_nwg_tree_demo") == 0)
    func = path_shortest_nwg_tree_demo;
  else if (strcmp(fname, "console_demo") == 0)
    func = console_demo;
  else if (strcmp(fname, "maze_shortest_distance_demo") == 0)
    func = maze_shortest_distance_demo;
  else if (strcmp(fname, "maze_shortest_distance") == 0)
    func = maze_shortest_distance;
  else if (strcmp(fname, "path_shortest_demo") == 0)
    func = path_shortest_demo;
  else if (strcmp(fname, "path_permutation_demo") == 0)
    func = path_permutation_demo;
  else if (strcmp(fname, "path_permutation_buffered_demo") == 0)
    func = path_permutation_buffered_demo;
  else if (strcmp(fname, "path_permutation_pool_demo") == 0)
    func = path_permutation_pool_demo;
  else if (strcmp(fname, "path_permutation_pipeline_demo") == 0)
    func = path_permutation_pipeline_demo;
  else if (strcmp(fname, "maze_solution_demo") == 0)
    func = maze_solution_demo;
  else if (strcmp(fname, "maze_solution") == 0)
    func = maze_solution;

  if (func == NULL) {
    fprintf(stderr, "%s not found \n", fname);
    return EXIT_FAILURE;
  }
  func();

  return EXIT_SUCCESS;
}
