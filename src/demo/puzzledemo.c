#include "puzzledemo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../crun/clocktime.h"
#include "../crun/pipeline.h"
#include "../crun/runpool.h"
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
  size_t rows = 7;
  size_t cols = 9;
  size_t elemstrlen = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
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
  int arr[] = {1, 2, 3};
  size_t size = sizeof(arr) / sizeof(arr[0]);
  Queue *queue = generate_permutations(arr, size);

  while (queue->size > 0) {
    int *arr = (int *)queue_dequeue(queue, NULL);
    for (size_t i = 0; i < size; i++) {
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
  size_t size;
  BufferQueue *bq;
} PermutationContext;

void permutation_consumer(void *context) {
  PermutationContext *ctx = (PermutationContext *)context;
  while (bufferq_can_read(ctx->bq)) {
    int *arr = (int *)bufferq_read(ctx->bq);
    if (arr == NULL) continue;

    for (size_t i = 0; i < ctx->size; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
    sleep(2);
  }
}

int path_permutation_buffered_demo(void) {
  printf("\n--------PUZZLE PERMUTATION BUFFERED DEMO----------\n");
  int arr[] = {1, 2, 3};
  size_t size = sizeof(arr) / sizeof(arr[0]);

  BufferQueue *bq = bufferq_create(4);
  bq->debug = true;
  Runtime *runtime = runtime_create();

  PermutationContext *context = malloc(sizeof(PermutationContext));
  context->size = size;
  context->bq = bq;
  runtime_exec(runtime, permutation_consumer, context);

  generate_permutations_buffered(bq, arr, size);

  runtime_join_destroy(runtime);  // should be called before bufferq_destroy
  bufferq_destroy(bq, free_data_func);

  printf("\n--------PUZZLE PERMUTATION BUFFERED DEMO----------\n");
  return EXIT_SUCCESS;
}

/*--------PUZZLE PERMUTATION BUFFERED DEMO----------*/

/*--------PUZZLE PERMUTATION BUFFERED POOL DEMO----------*/

int path_permutation_pool_demo(void) {
  printf("\n------PUZZLE PERMUTATION BUFFERED POOL DEMO-------\n");
  int arr[] = {1, 2, 3, 4};
  size_t size = sizeof(arr) / sizeof(arr[0]);

  BufferQueue *bq = bufferq_create(10);
  bq->debug = true;
  Runpool *runpool = runpool_create(3);

  PermutationContext *context = malloc(sizeof(PermutationContext));
  context->size = size;
  context->bq = bq;
  runpool_exec(runpool, permutation_consumer, context);
  runpool_exec(runpool, permutation_consumer, context);
  runpool_exec(runpool, permutation_consumer, context);

  generate_permutations_buffered(bq, arr, size);

  runpool_join_destroy(runpool);  // should be called before bufferq_destroy
  bufferq_destroy(bq, free_data_func);

  printf("\n------PUZZLE PERMUTATION BUFFERED POOL DEMO------\n");
  return EXIT_SUCCESS;
}

/*--------PUZZLE PERMUTATION BUFFERED POOL DEMO----------*/

/*--------PUZZLE PERMUTATION PIPELINE DEMO----------*/

void path_permutation_producer(BufferQueue *bq, void *context) {
  size_t arrsize = *(size_t *)context;
  int arr[arrsize];

  for (size_t i = 0; i < arrsize; i++) {
    arr[i] = i + 1;
  }

  generate_permutations_buffered(bq, arr, arrsize);
}

void path_permutation_consumer(BufferQueue *bq, void *context) {
  size_t arrsize = *(size_t *)context;

  while (bufferq_can_read(bq)) {
    int *arr = (int *)bufferq_read(bq);
    if (arr == NULL) continue;

    for (size_t i = 0; i < arrsize; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
    sleep(2);
  }
}

int path_permutation_pipeline_demo(void) {
  printf("\n------PUZZLE PERMUTATION PIPELINE DEMO-------\n");

  // 1 producer, 3 consumers, 4 work capacity
  Pipeline *pipe = pipeline_create(1, 3, 4);
  pipeline_debug(pipe);

  size_t *arrsize = malloc(sizeof(size_t));
  *arrsize = 4;

  // 1 producer
  pipeline_add_producer(pipe, path_permutation_producer, arrsize);

  // 3 consumers
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);

  pipeline_join_destory(pipe, NULL);
  free(arrsize);

  printf("\n------PUZZLE PERMUTATION PIPELINE DEMO------\n");
  return EXIT_SUCCESS;
}

/*--------PUZZLE PERMUTATION PIPELINE DEMO----------*/

/*--------PUZZLE MAZE SULUTION DEMO----------*/

int maze_solution_demo(void) {
  printf("\n-----------PUZZLE PMAZE SULUTION DEMO-----------\n");

  ClockTime *time = clocktime_create();
  clocktime_start(time);

  const char maze[] = "##########....#.@##.@...G.##.#.....##.##@#####....S..##########";
  // const char maze[] = "##########..@.#.@##@....G.##.#@...@##.##@#####..@.S..##########";
  // const char maze[] = "##########..@.#.@##@..@.G.##.#@...@##.##@#####..@.S.@##########";
  size_t rows = 7;
  size_t cols = 9;

  // const char maze[] =
  //     "###################................##...S............##................##.############...##"
  //     "....@...........##....##############..#######..G....##................##................###"
  //     "################";
  // const char maze[] =
  //     "###################................##...S............##................##.############...##"
  //     "................##....##############..#######..G....##................##................###"
  //     "################";
  // size_t rows = 11;
  // size_t cols = 18;

  size_t elemstrlen = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      char temp[elemstrlen];
      char data = maze[i * cols + j];
      snprintf(temp, elemstrlen, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  printf("\n");

  MazeData *mazedata = maze_prepare_data(arr, rows, cols, elemstrlen);
  maze_search_solution(mazedata);
  free_maze_data_func(mazedata);

  printf("Time taken: %f sec\n", clocktime_stop(time));
  clocktime_destroy(time);

  printf("\n-----------PUZZLE PMAZE SULUTION DEMO-----------\n");
  return EXIT_SUCCESS;
}

/*--------PUZZLE PERMUTATION PIPELINE DEMO----------*/
