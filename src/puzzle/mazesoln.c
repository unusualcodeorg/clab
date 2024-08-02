#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../crun/clocktime.h"
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
      printf("%s", patharr[i][j]);
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
    printf("\n");
  }
}

void free_stack_data(void *data) {
  Stack *stack = (Stack *)data;
  stack_destroy(stack, free_data_func);
}

void free_maze_data_func(void *data) {
  MazeData *mazedata = (MazeData *)data;
  list_destroy(mazedata->cpindexes, free_data_func);
  queue_destroy(mazedata->solution, NULL);  // contains location

  hashmap_destroy(mazedata->gmap->idmap, free_data_func);
  hashmap_destroy(mazedata->cache, free_stack_data);

  graph_destroy(mazedata->gmap->graph, free_location_data_func);  // free location
  util_destroy_2d_str_arr(mazedata->arr, mazedata->rows, mazedata->cols);

  free(mazedata->gmap);
  free(mazedata);
}

void maze_permutation_consumer(BufferQueue *bq, void *context) {
  MazeData *mazedata = (MazeData *)context;

  while (bufferq_can_read(bq)) {
    int *arr = (int *)bufferq_read(bq);
    if (arr == NULL) continue;

    /**
     * S 1 2 3 G : mazedata->cpindexes contains indexes for - 1, 2, 3
     * We generate permisations on mazedata->cpindexes
     * We find pair waise short distances: (S, 1),(1, 2),(2, 3),(3, G)
     */
    unsigned int distance = 0;
    Queue *queue = queue_create();
    for (unsigned int k = 0; k <= mazedata->cpindexes->size; k++) {
      char *srckey, *destkey;
      unsigned int index_first, index_second;
      unsigned int i_first, j_first;
      unsigned int i_second, j_second;

      if (k == 0) {
        index_first = mazedata->srcindex;
        index_second = arr[k];
      } else if (k == mazedata->cpindexes->size) {
        index_first = arr[k - 1];
        index_second = mazedata->destindex;
      } else {
        index_first = arr[k - 1];
        index_second = arr[k];
      }

      i_first = index_first / mazedata->cols;
      j_first = index_first % mazedata->cols;
      srckey = mazedata->arr[i_first][j_first];

      i_second = index_second / mazedata->cols;
      j_second = index_second % mazedata->cols;
      destkey = mazedata->arr[i_second][j_second];

      unsigned int srcid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, srckey);
      unsigned int destid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, destkey);

      /**
       * caching the pair waise distance since it is quite a small number
       * but will reduce a lot of processing
       */
      char *uidkey = cantor_pairing_uid_str(srcid, destid);
      Stack *stack = hashmap_get(mazedata->cache, uidkey);
      if (stack == NULL) {
        stack = path_find_shortest(mazedata->gmap->graph, srcid, destid);
        hashmap_put(mazedata->cache, uidkey, stack);
      }

      Location *dest = stack_get(stack, stack->size - 1);
      distance += dest->cost;
      queue_enqueue(queue, stack);
    }

    if (distance < mazedata->mindistance) {
      queue_destroy(mazedata->solution, NULL);  // stack lives in hashmap
      mazedata->mindistance = distance;
      mazedata->solution = queue;
    } else {
      queue_destroy(queue, NULL);
    }
  }
}

void maze_permutation_producer(BufferQueue *bq, void *context) {
  MazeData *mazedata = (MazeData *)context;
  unsigned int arr[mazedata->cpindexes->size];

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
  printf("\nSearching: please wait...\n");
  // only source and destination case
  if (mazedata->cpindexes->size == 0) {
    unsigned int srcid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, mazedata->src);
    unsigned int dstid = *(unsigned int *)hashmap_get(mazedata->gmap->idmap, mazedata->dest);

    Stack *stack = path_find_shortest(mazedata->gmap->graph, srcid, dstid);
    maze_sd_result_print(stack, mazedata->arr, mazedata->rows, mazedata->cols);
    stack_destroy(stack, free_data_func);
  } else {
    unsigned int conscount = 10;  // 10 threads
    unsigned int capacity = 1000;
    Pipeline *pipe = pipeline_create(1, conscount, capacity);
    pipeline_add_producer(pipe, maze_permutation_producer, mazedata);
    for (unsigned int i = 0; i < conscount; i++) {
      pipeline_add_consumer(pipe, maze_permutation_consumer, mazedata);
    }

    pipeline_join_destory(pipe, free_data_func);

    if (mazedata->mindistance < INT_MAX) {
      maze_solution_result_print(mazedata);
      printf("Maze: shortest travel distance %d\n", mazedata->mindistance);
    } else {
      printf("Maze: can't find shortest travel distance\n");
    }
  }
}

MazeData *maze_prepare_data(char ***arr, unsigned int rows, unsigned int cols,
                            unsigned int elemstrlen) {
  List *cpindexes = list_create();

  char *src = "S";
  char *dest = "G";
  char *skip = "#";
  char *checkpoint = "@";

  printf("Maze: Making Checkpoint Unique\n");

  unsigned int srcindex = 0;
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

      if (strcmp(data, src) == 0) {
        srcindex = index;
      } else if (strcmp(data, dest) == 0) {
        destindex = index;
      }

      printf("%s", data);
    }
    printf("\n");
  }

  if (srcindex == 0) {
    perror("S is not properly placed in the maze");
    exit(EXIT_FAILURE);
  }

  if (destindex == 0) {
    perror("G is not properly placed in the maze");
    exit(EXIT_FAILURE);
  }

  // nC2 = (n (n - 1)) / 2 [add 1 in pairs for the rounding]
  unsigned int pairs = ((cpindexes->size + 2) * (cpindexes->size + 1)) / 2 + 1;
  MazeData *mazedata = (MazeData *)malloc(sizeof(MazeData));
  mazedata->arr = arr;
  mazedata->rows = rows;
  mazedata->cols = cols;
  mazedata->mindistance = UINT_MAX;
  mazedata->cpindexes = cpindexes;
  mazedata->solution = queue_create();
  mazedata->cache = hashmap_create(pairs);
  mazedata->srcindex = srcindex;
  mazedata->destindex = destindex;
  mazedata->src = src;
  mazedata->dest = dest;
  mazedata->skip = skip;
  mazedata->cp = checkpoint;
  mazedata->gmap =
      maze_graph_map_create(mazedata->arr, mazedata->rows, mazedata->cols, mazedata->skip);

  graph_print(mazedata->gmap->graph, location_int_data_to_string);

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

  ClockTime *time = clocktime_create();
  clocktime_start(time);

  printf("\nYour input %dx%d array:\n", rows, cols);

  MazeData *mazedata = maze_prepare_data(arr, rows, cols, elemstrlen);
  maze_search_solution(mazedata);
  free_maze_data_func(mazedata);

  printf("Time taken: %f sec\n", clocktime_stop(time));
  clocktime_destroy(time);

  return EXIT_SUCCESS;
}
