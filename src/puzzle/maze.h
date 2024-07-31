
#include "../dslib/list.h"
#include "../dslib/queue.h"
#include "../dslib/stack.h"
#include "../dslib/util.h"

#ifndef CLAB_PUZZLE_MAZE_H
#define CLAB_PUZZLE_MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char ***arr;
  unsigned int rows;
  unsigned int cols;
  Queue *solutions;
  List *cpindexes;
  char *start;
  char *cp;  // checkpoints
  char *dest;
  char *skip;
} MazeData;

void free_maze_data_func(void *data);
int maze_shortest_distance(void);
int maze_solution(void);

// skip defines the char which can not be traversed
void maze_find_shortest_distance(char ***arr, unsigned int rows, unsigned int cols, char *start,
                                 char *dest, char *skip);

Graph2DMap *maze_graph_map_create(char ***arr, unsigned int rows, unsigned int cols, char *skip);

void maze_sd_result_print(Stack *stack, char ***arr, unsigned int rows, unsigned int cols);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_H
