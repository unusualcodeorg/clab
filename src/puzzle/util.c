#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dslib/stack.h"

void free_permutation_state(void *data) {
  PermutationState *state = (PermutationState *)data;
  free(state->arr);
  free(state);
}

void swap(int *x, int *y) {
  int temp = *x;
  *x = *y;
  *y = temp;
}

PermutationState *create_permutation_state(int *arr, int n, int left, int right) {
  PermutationState *state = (PermutationState *)malloc(sizeof(PermutationState));
  state->arr = (int *)malloc(n * sizeof(int));
  memcpy(state->arr, arr, n * sizeof(int));  // Copy the array to avoid aliasing
  state->left = left;
  state->right = right;
  return state;
}

void generate_permutations(int *arr, int n) {
  Stack *stack = stack_create();

  // Push the initial state onto the stack
  PermutationState *initial = create_permutation_state(arr, n, 0, n - 1);
  stack_push(stack, initial);

  while (stack->size > 0) {
    PermutationState *state = (PermutationState *)stack_pop(stack, NULL);

    if (state->left == state->right) {
      for (int i = 0; i <= state->right; i++) {
        printf("%d ", state->arr[i]);
      }
      printf("\n");
    } else {
      for (int i = state->left; i <= state->right; i++) {
        PermutationState *newstate =
            create_permutation_state(state->arr, n, state->left + 1, state->right);

        // Swap elements in the new state's array
        swap(&newstate->arr[state->left], &newstate->arr[i]);

        // Push the new state onto the stack
        stack_push(stack, newstate);
      }
    }

    free_permutation_state(state);
  }

  stack_destroy(stack, free_permutation_state);
}
