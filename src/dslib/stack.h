#include <pthread.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"

#ifndef CLAB_DS_LIB_STACK_H
#define CLAB_DS_LIB_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StackNode {
  void *data;
  struct StackNode *next;
} StackNode;

typedef struct {
  StackNode *top;
  size_t size;
  pthread_rwlock_t rwlock;
} Stack;

Stack *stack_create(void);
void stack_push(Stack *stack, void *data);
void *stack_pop(Stack *stack, FreeDataFunc freedatafunc);
void *stack_peek(Stack *stack);
void *stack_get(Stack *stack, size_t position);
void stack_print(Stack *stack, DataToString tostring);
void stack_destroy(Stack *stack, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_STACK_H
