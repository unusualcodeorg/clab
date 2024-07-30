#include <pthread.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"

#ifndef CLAB_DS_LIB_QUEUE_H
#define CLAB_DS_LIB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

typedef struct {
  QueueNode *start;
  QueueNode *end;
  unsigned int size;
  pthread_rwlock_t rwlock;
} Queue;

Queue *queue_create(void);
void queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue, FreeDataFunc freedatafunc);
void *queue_peek(Queue *queue);
void *queue_get(Queue *queue, unsigned int position);
void queue_print(Queue *queue, DataToString tostring);
void queue_destroy(Queue *queue, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_QUEUE_H
