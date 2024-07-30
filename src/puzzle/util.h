
#include "../dslib/queue.h"

#ifndef CLAB_PUZZLE_MAZE_UTIL_H
#define CLAB_PUZZLE_MAZE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int *arr;
  int left;
  int right;
} PermutationState;

Queue *generate_permutations(int *arr, int n);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_UTIL_H
