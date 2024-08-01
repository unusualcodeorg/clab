#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../crun/pipeline.h"
#include "../dslib/datastr.h"
#include "../dslib/destroy.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "maze.h"
#include "util.h"

void maze_solution_step_print(MazeData *mazedata, Stack *stack) {
  char ***patharr = util_create_2d_str_arr(mazedata->rows, mazedata->cols, 5);

  for (unsigned int i = 0; i < mazedata->rows; i++) {
    for (unsigned int j = 0; j < mazedata->cols; j++) {
      if (strcmp(mazedata->arr[i][j], "#") != 0) {
        *patharr[i][j] = ' ';
        continue;
      }
      strcpy(patharr[i][j], mazedata->arr[i][j]);
    }
  }

  StackNode *node = stack->top;
  while (node) {
    Location *loc = (Location *)node->data;
    unsigned int position = *(unsigned int *)loc->data;
    unsigned i = position / mazedata->cols;
    unsigned j = position % mazedata->cols;
    strcpy(patharr[i][j], mazedata->arr[i][j]);
    node = node->next;
  }

  for (unsigned int i = 0; i < mazedata->rows; i++) {
    for (unsigned int j = 0; j < mazedata->cols; j++) {
      printf("%c", *patharr[i][j]);
    }
    printf("\n");
  }

  util_destroy_2d_str_arr(patharr, mazedata->rows, mazedata->cols);
}

/**
 * Q : S1: A-x-x-x-B, S2: B-x-x-C, S3: C-x-x-x-D
 */
void maze_solution_result_print(MazeData *mazedata) {
  QueueNode *qnode = mazedata->solution->start;
  int counter = 0;
  while (qnode) {
    printf("Step %d:\n", ++counter);
    Stack *stack = qnode->data;
    maze_solution_step_print(mazedata, stack);
    qnode = qnode->next;
  }
}

void free_queue_stacks(Queue *queue) {
  Stack *stack = queue_dequeue(queue, NULL);
  while (stack) {
    // only free the stack->data (location) and not stack->data->data
    stack_destroy(stack, free_data_func);
    stack = queue_dequeue(queue, NULL);
  }
  queue_destroy(queue, free_data_func);
}

void free_maze_data_func(void *data) {
  MazeData *mazedata = (MazeData *)data;
  list_destroy(mazedata->cpindexes, free_data_func);
  free_queue_stacks(mazedata->solution);                          // contains location
  graph_destroy(mazedata->gmap->graph, free_location_data_func);  // free location
  hashmap_destroy(mazedata->gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(mazedata->arr, mazedata->rows, mazedata->cols);

  free(mazedata->gmap);
  free(mazedata);
}

void maze_permutation_consumer(BufferQueue *bq, void *context) {
  MazeData *mazedata = (MazeData *)context;

  while (bufferq_is_open(bq)) {
    int *arr = (int *)bufferq_consume(bq);
    if (arr == NULL) continue;

    unsigned int distance = 0;
    Queue *queue = queue_create();
    for (unsigned int k = 0; k < mazedata->cpindexes->size - 1; k++) {
      unsigned int index = *(unsigned int *)list_get_at(mazedata->cpindexes, k);
      unsigned i = index / mazedata->cols;
      unsigned j = index % mazedata->cols;
      char *srckey = mazedata->arr[i][j];

      index = *(unsigned int *)list_get_at(mazedata->cpindexes, k + 1);
      i = index / mazedata->cols;
      j = index % mazedata->cols;
      char *destkey = mazedata->arr[i][j];

      // TODO: can be optimized by only calculating the pairs which are new in the permutation
      unsigned int srcid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, srckey);
      unsigned int destid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, destkey);

      Stack *stack = path_find_shortest(mazedata->gmap->graph, srcid, destid);
      Location *dest = stack_get(stack, stack->size - 1);
      distance += dest->cost;
      queue_enqueue(queue, stack);
    }

    if (distance < mazedata->mindistance) {
      free_queue_stacks(mazedata->solution);
      mazedata->mindistance = distance;
      mazedata->solution = queue;
    } else {
      free_queue_stacks(queue);
    }
  }
}

void maze_permutation_producer(BufferQueue *bq, void *context) {
  MazeData *mazedata = (MazeData *)context;
  int arr[mazedata->cpindexes->size];

  for (unsigned int i = 0; i < mazedata->cpindexes->size; i++) {
    unsigned int *index = list_get_at(mazedata->cpindexes, i);
    if (index == NULL) {
      perror("List of checkpoints does not have all the indexes");
      exit(EXIT_FAILURE);
    }
    arr[i] = *index;
  }

  generate_permutations_buffered(bq, arr, mazedata->cpindexes->size);
}

void maze_search_solution(MazeData *mazedata) {
  if (mazedata->cpindexes->size < 2) {
    perror("maze should have both S and G");
    exit(EXIT_FAILURE);
  }

  // only source and destination case
  if (mazedata->cpindexes->size == 2) {
    unsigned int srcid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, mazedata->start);
    unsigned int dstid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, mazedata->dest);

    Stack *stack = path_find_shortest(mazedata->gmap->graph, srcid, dstid);
    maze_sd_result_print(stack, mazedata->arr, mazedata->rows, mazedata->cols);
    stack_destroy(stack, free_data_func);
  } else {
    Pipeline *pipe = pipeline_create(1, 1, 4);
    pipeline_add_producer(pipe, maze_permutation_producer, mazedata);
    pipeline_add_consumer(pipe, maze_permutation_consumer, mazedata);
    pipeline_join_destory(pipe, NULL);

    if (mazedata->mindistance < INT_MAX) {
      maze_solution_result_print(mazedata);
      printf("Maze: shortest travel distance %d :D", mazedata->mindistance);
    } else {
      printf("Maze: can't find shortest travel distance :(");
    }
  }
}

MazeData *maze_prepare_data(char ***arr, unsigned int rows, unsigned int cols,
                            unsigned int elemstrlen) {
  List *cpindexes = list_create();

  char *start = "S";
  char *dest = "G";
  char *skip = "#";
  char *checkpoint = "@";

  printf("Maze: Make Checkpoint Unique\n");

  unsigned int startindex = 0;
  unsigned int destindex = 0;
  unsigned int count = 0;
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      char *data = arr[i][j];
      unsigned int index = i * cols + j;

      if (strcmp(data, checkpoint) == 0) {
        unsigned int *cindex = malloc(sizeof(unsigned int));
        *cindex = index;
        list_add(cpindexes, cindex);
      }

      if (strcmp(data, checkpoint) == 0) {
        snprintf(data, elemstrlen, "%d", ++count);  // rename for search
      } else {
        snprintf(data, elemstrlen, "%s", data);
      }

      if (strcmp(data, start) == 0) {
        startindex = index;
      } else if (strcmp(data, dest) == 0) {
        destindex = index;
      }

      printf("%s", data);
    }
    printf("\n");
  }
  printf("\n");

  if (startindex == 0) {
    perror("S is not properly placed in the maze");
    exit(EXIT_FAILURE);
  } else {
    unsigned int *cindex = malloc(sizeof(unsigned int));
    *cindex = startindex;
    list_add_at(cpindexes, cindex, 0);
  }

  if (destindex == 0) {
    perror("G is not properly placed in the maze");
    exit(EXIT_FAILURE);
  } else {
    unsigned int *cindex = malloc(sizeof(unsigned int));
    *cindex = destindex;
    list_add(cpindexes, cindex);
  }

  MazeData *mazedata = (MazeData *)malloc(sizeof(MazeData));
  mazedata->arr = arr;
  mazedata->rows = rows;
  mazedata->cols = cols;
  mazedata->mindistance = UINT_MAX;
  mazedata->cpindexes = cpindexes;
  mazedata->solution = queue_create();
  mazedata->start = start;
  mazedata->dest = dest;
  mazedata->skip = skip;
  mazedata->cp = checkpoint;
  mazedata->gmap =
      maze_graph_map_create(mazedata->arr, mazedata->rows, mazedata->cols, mazedata->skip);

  return mazedata;
}

int maze_solution(void) {
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

  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      /**
       * space before %d tells scanf to ignore any leading whitespace characters
       * (including newlines)
       */
      scanf(" %c", arr[i][j]);
    }
  }

  printf("\nYour input %dx%d array:\n", rows, cols);

  MazeData *mazedata = maze_prepare_data(arr, rows, cols, elemstrlen);
  maze_search_solution(mazedata);
  free_maze_data_func(mazedata);

  return EXIT_SUCCESS;
}
