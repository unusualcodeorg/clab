#include "destroy.h"

#include <stdlib.h>

void free_data_func(void *data) {
  if (data != NULL) free(data);
}

void free_queue_stacks(Queue *queue, FreeDataFunc freedatafunc) {
  Stack *stack = queue_dequeue(queue, NULL);
  while (stack) {
    stack_destroy(stack, freedatafunc);
    stack = queue_dequeue(queue, NULL);
  }
  queue_destroy(queue, NULL);
}
