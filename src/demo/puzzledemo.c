#include "puzzledemo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../crun/runtime.h"
#include "../dslib/bufferq.h"
#include "../dslib/util.h"
#include "../puzzle/maze.h"
#include "../puzzle/util.h"

/*------------PUZZLE SHORTEST DISTANCE DEMO--------------*/
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
  printf("\n------------PUZZLE SHORTEST DISTANCE DEMO--------------\n");
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

  printf("\n------------PUZZLE SHORTEST DISTANCE DEMO--------------\n");
  return EXIT_SUCCESS;
}

/*------------PUZZLE SHORTEST DISTANCE DEMO--------------*/

/*------------PUZZLE PERMUTATION DEMO--------------*/

int path_permutation_demo(void) {
  printf("\n------------PUZZLE PERMUTATION DEMO--------------\n");
  int arr[] = {1, 2, 3, 4, 5};
  int size = sizeof(arr) / sizeof(arr[0]);
  Queue *queue = generate_permutations(arr, size);

  while (queue->size > 0) {
    int *arr = (int *)queue_dequeue(queue, NULL);
    for (int i = 0; i < size; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
  }

  queue_destroy(queue, free_data_func);

  printf("\n------------PUZZLE PERMUTATION DEMO--------------\n");
  return EXIT_SUCCESS;
}

/*------------PUZZLE PERMUTATION DEMO--------------*/

/*--------PUZZLE PERMUTATION BUFFERED DEMO----------*/

typedef struct {
  int size;
  BufferQueue *bq;
} PermutationContext;

void permutation_consumer(void *context) {
  PermutationContext *ctx = (PermutationContext *)context;
  while (bufferq_can_consume(ctx->bq) == true) {
    int *arr = (int *)bufferq_consume(ctx->bq);
    if (arr == NULL) continue;

    for (int i = 0; i < ctx->size; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
  }
}

int path_permutation_buffered_demo(void) {
  printf("\n--------PUZZLE PERMUTATION BUFFERED DEMO----------\n");
  int arr[] = {1, 2, 3, 4, 5};
  int size = sizeof(arr) / sizeof(arr[0]);

  BufferQueue *bq = bufferq_create(10);
  bq->debug = true;
  Runtime *runtime = runtime_create("Buffer", false);

  PermutationContext *context = malloc(sizeof(PermutationContext));
  context->size = size;
  context->bq = bq;
  runtime_exec(runtime, permutation_consumer, context);

  generate_permutations_buffered(bq, arr, size);

  runtime_destroy(runtime);  // should be called before bufferq_destroy
  bufferq_destroy(bq, free_data_func);

  printf("\n--------PUZZLE PERMUTATION BUFFERED DEMO----------\n");
  return EXIT_SUCCESS;
}

/*--------PUZZLE PERMUTATION BUFFERED DEMO----------*/
