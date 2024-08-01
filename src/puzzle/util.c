#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dslib/destroy.h"
#include "../dslib/queue.h"
#include "../dslib/stack.h"

void swap(unsigned int *x, unsigned int *y) {
  unsigned int temp = *x;
  *x = *y;
  *y = temp;
}

PermutationState *create_permutation_state(unsigned int *arr, unsigned int n, unsigned int left,
                                           unsigned int right) {
  PermutationState *state = (PermutationState *)malloc(sizeof(PermutationState));
  state->arr = (unsigned int *)malloc(n * sizeof(unsigned int));
  memcpy(state->arr, arr, n * sizeof(unsigned int));  // Copy the array to avoid aliasing
  state->left = left;
  state->right = right;
  return state;
}

Queue *generate_permutations(unsigned int *arr, unsigned int n) {
  if (n > 10) {
    perror("permutation larger than 10 will take a lot of memory");
    exit(EXIT_FAILURE);
  }

  Stack *stack = stack_create();
  Queue *queue = queue_create();

  // Push the initial state onto the stack
  PermutationState *initial = create_permutation_state(arr, n, 0, n - 1);
  stack_push(stack, initial);

  while (stack->size > 0) {
    PermutationState *state = (PermutationState *)stack_pop(stack, NULL);

    if (state->left == state->right) {
      queue_enqueue(queue, state->arr);
      continue;
    }

    for (unsigned int i = state->left; i <= state->right; i++) {
      PermutationState *newstate =
          create_permutation_state(state->arr, n, state->left + 1, state->right);

      // Swap elements in the new state's array
      swap(&newstate->arr[state->left], &newstate->arr[i]);

      // Push the new state onto the stack
      stack_push(stack, newstate);
    }

    // only free the state and not the arr as it is passed to the queue
    free(state);
  }

  // only free the state and not the arr as it is passed to the queue
  stack_destroy(stack, free_data_func);
  return queue;
}

void generate_permutations_buffered(BufferQueue *bq, unsigned int *arr, unsigned int arrsize) {
  if (arrsize > 9) {
    perror("permutation larger than 10 will take a lot of memory");
    exit(EXIT_FAILURE);
  }

  Stack *stack = stack_create();
  PermutationState *initial = create_permutation_state(arr, arrsize, 0, arrsize - 1);
  stack_push(stack, initial);

  while (stack->size > 0) {
    PermutationState *state = (PermutationState *)stack_pop(stack, NULL);

    if (state->left == state->right) {
      bufferq_write(bq, state->arr);
      continue;
    }

    for (unsigned int i = state->left; i <= state->right; i++) {
      PermutationState *newstate =
          create_permutation_state(state->arr, arrsize, state->left + 1, state->right);
      swap(&newstate->arr[state->left], &newstate->arr[i]);
      stack_push(stack, newstate);
    }

    free(state);
  }

  stack_destroy(stack, free_data_func);
  bufferq_close_writer(bq);
}
