#include "puzzledemo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dslib/util.h"
#include "../puzzle/maze.h"
#include "../puzzle/util.h"

/*
#########
#..@.#.@#
#@....G.#
#.#..@.@#
#.##@####
#..@.S..#
#########
*/
int maze_shortest_distance_demo(void) {
  const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";
  unsigned int rows = 7;
  unsigned int cols = 9;
  unsigned int elemstrlen = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);

  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      char temp[elemstrlen];
      char data = maze[i * cols + j];
      snprintf(temp, elemstrlen, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  maze_find_shortest_distance(arr, rows, cols, "S", "G", "#");

  util_destroy_2d_str_arr(arr, rows, cols);
  return EXIT_SUCCESS;
}

/*------------PATH PERMUTATION DEMO--------------*/

int path_permutation_demo(void) {
  int arr[] = {1, 2, 3};
  int arrsize = sizeof(arr) / sizeof(arr[0]);
  Queue *queue = generate_permutations(arr, arrsize);

  while (queue->size > 0) {
    QueueNode *node = queue_dequeue(queue, NULL);
    int *resultarr = (int *)node->data;
    for (int i = 0; i < arrsize; i++) {
      printf("%d ", resultarr[i]);
    }
    printf("\n");
  }

  queue_destroy(queue, free_data_func);
  return EXIT_SUCCESS;
}

/*------------SPATH PERMUTATION DEMO--------------*/
