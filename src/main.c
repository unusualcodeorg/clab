#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug/dfunc.h"
#include "demo/dmcrun.h"
#include "demo/dmdslib.h"
#include "demo/dmpuzzle.h"
#include "demo/dmterm.h"

enum COMMAND {
  NONE,
  ALL,
  STACK,
  QUEUE,
  RUNTIME,
  GRAPH,
  TREE,
  HASHMAP,
  CONSOLE,
  PATH,
  PUZZLE_MAZE_SD,
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "error  - one command needed [all, debug, stack, queue, runtime, graph, tree, "
            "hashmap, console, path]\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "debug") == 0) {
    if (argc < 3) {
      fprintf(stderr, "error  - debug require function name\n");
      return EXIT_FAILURE;
    }
    return debugfn(argv[2]);
  }

  int option = NONE;
  if (strcmp(argv[1], "all") == 0) option = ALL;
  if (strcmp(argv[1], "stack") == 0)
    option = STACK;
  else if (strcmp(argv[1], "queue") == 0)
    option = QUEUE;
  else if (strcmp(argv[1], "runtime") == 0)
    option = RUNTIME;
  else if (strcmp(argv[1], "graph") == 0)
    option = GRAPH;
  else if (strcmp(argv[1], "tree") == 0)
    option = TREE;
  else if (strcmp(argv[1], "hashmap") == 0)
    option = HASHMAP;
  else if (strcmp(argv[1], "console") == 0)
    option = CONSOLE;
  else if (strcmp(argv[1], "path") == 0)
    option = PATH;
  else if (strcmp(argv[1], "puzzle-maze-sd") == 0)
    option = PUZZLE_MAZE_SD;

  switch (option) {
    case ALL:
      stack_demo();
      stack_concurrent_demo();
      queue_demo();
      queue_concurrent_demo();
      runtime_demo();
      runpool_demo();
      graph_demo();
      graph_concurrent_demo();
      graph_2d_arr_demo();
      tree_demo();
      hashmap_demo();
      path_shortest_nw_graph_demo();
      break;
    case STACK:
      stack_demo();
      stack_concurrent_demo();
      break;
    case QUEUE:
      queue_demo();
      queue_concurrent_demo();
      break;
    case RUNTIME:
      runtime_demo();
      runpool_demo();
      break;
    case GRAPH:
      graph_demo();
      graph_concurrent_demo();
      graph_2d_arr_demo();
      break;
    case TREE:
      tree_demo();
      break;
    case HASHMAP:
      hashmap_demo();
      break;
    case CONSOLE:
      console_demo();
      break;
    case PATH:
      path_shortest_nw_graph_demo();
      break;
    case PUZZLE_MAZE_SD:
      demo_maze_shortest_distance();
      break;
    default:
      fprintf(stderr, "Unknown command name: %s\n", argv[1]);
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
