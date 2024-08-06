
#include "../dslib/bufferq.h"
#include "../dslib/queue.h"

#ifndef CLAB_PUZZLE_MAZE_UTIL_H
#define CLAB_PUZZLE_MAZE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t *arr;
  size_t left;
  size_t right;
} PermutationState;

Queue *generate_permutations(size_t *arr, size_t n);
void generate_permutations_buffered(BufferQueue *bq, size_t *arr, size_t arrsize);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_UTIL_H
