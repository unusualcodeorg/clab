
#include "../dslib/stack.h"
#include "../dslib/util.h"

#ifndef CLAB_PUZZLE_MAZE_H
#define CLAB_PUZZLE_MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

int maze_shortest_distance(void);
Graph2DMap *maze_graph_map_create(char ***arr, int rows, int cols, bool autofree);
void maze_sd_result_print(Stack *stack, char ***arr, int cols);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_H
