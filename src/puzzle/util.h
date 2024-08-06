
#include "../dslib/bufferq.h"
#include "../dslib/queue.h"

#ifndef CLAB_PUZZLE_MAZE_UTIL_H
#define CLAB_PUZZLE_MAZE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int *arr;
  size_t left;
  size_t right;
} PermutationState;

Queue *generate_permutations(int *arr, size_t arrsize);
void generate_permutations_buffered(BufferQueue *bq, int *arr, size_t arrsize);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_UTIL_H
