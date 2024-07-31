#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../crun/pipeline.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "maze.h"
#include "util.h"

void free_maze_data_func(void *data) {
  MazeData *mazedata = (MazeData *)data;
  free(mazedata->arr);
  list_destroy(mazedata->cpindexes, free_data_func);

  Stack *stack = queue_dequeue(mazedata->solutions, NULL);
  while (stack) {
    stack_destroy(stack, NULL);
    stack = queue_dequeue(mazedata->solutions, NULL);
  }

  queue_destroy(mazedata->solutions, free_data_func);
  free(mazedata);
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

void maze_permutation_consumer(BufferQueue *bq, void *context) {
  MazeData *mazedata = (MazeData *)context;

  while (bufferq_is_open(bq)) {
    // TODO
    int *arr = (int *)bufferq_consume(bq);
    if (arr == NULL) continue;

    for (unsigned int i = 0; i < mazedata->cpindexes->size; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
  }
}

void maze_find_solution(MazeData *mazedata) {
  Graph2DMap *gmap =
      maze_graph_map_create(mazedata->arr, mazedata->rows, mazedata->cols, mazedata->skip);

  unsigned int srcid = *(unsigned int *)hashmap_get(gmap->idmap, mazedata->start);
  unsigned int dstid = *(unsigned int *)hashmap_get(gmap->idmap, mazedata->dest);

  Stack *stack = path_find_shortest(gmap->graph, srcid, dstid);

  stack_destroy(stack, NULL);
  graph_destroy(gmap->graph, free_data_func);  // location data will be free with arr free
  hashmap_destroy(gmap->idmap, free_data_func);

  Pipeline *pipe = pipeline_create(1, 1, 4);
  pipeline_add_producer(pipe, maze_permutation_producer, mazedata);
  pipeline_add_consumer(pipe, maze_permutation_consumer, mazedata);

  pipeline_join_destory(pipe, NULL);
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
  char maze[rows][cols];
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      /**
       * space before %d tells scanf to ignore any leading whitespace characters
       * (including newlines)
       */
      scanf(" %c", &maze[i][j]);
    }
  }

  printf("\nYour input %dx%d array:\n", rows, cols);

  char ***arr = util_create_2d_str_arr(rows, cols, elemstrlen);
  List *cpindexes = list_create();

  char *start = "S";
  char *dest = "G";
  char *skip = "#";
  char *checkpoint = "@";

  unsigned int startindex = 0;
  unsigned int destindex = 0;
  unsigned int count = 0;
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      char data = maze[i][j];
      char buffer[elemstrlen];

      if (strcmp(buffer, checkpoint) == 0) {
        snprintf(buffer, elemstrlen, "%c%d", data, ++count);  // rename for search
      } else {
        snprintf(buffer, elemstrlen, "%c", data);
      }

      unsigned int index = i * cols + j;

      if (strcmp(buffer, start) == 0) {
        startindex = index;
      } else if (strcmp(buffer, dest) == 0) {
        destindex = index;
      }

      if (strcmp(buffer, checkpoint) == 0) {
        unsigned int *cindex = malloc(sizeof(unsigned int));
        *cindex = index;
        list_add(cpindexes, cindex);
      }

      strcpy(arr[i][j], buffer);
      printf("%c", data);
    }
    printf("\n");
  }

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
  mazedata->cpindexes = cpindexes;
  mazedata->solutions = queue_create();  // queue of stacks
  mazedata->start = start;
  mazedata->dest = dest;
  mazedata->skip = skip;
  mazedata->cp = checkpoint;

  maze_find_solution(mazedata);

  util_destroy_2d_str_arr(arr, rows, cols);
  return EXIT_SUCCESS;
}
