
#include "../dslib/bufferq.h"
#include "../dslib/queue.h"

#ifndef CLAB_PUZZLE_MAZE_UTIL_H
#define CLAB_PUZZLE_MAZE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int *arr;
  unsigned int left;
  unsigned int right;
} PermutationState;

Queue *generate_permutations(unsigned int *arr, unsigned int n);
void generate_permutations_buffered(BufferQueue *bq, unsigned int *arr, unsigned int arrsize);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_UTIL_H
