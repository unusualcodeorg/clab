#include <pthread.h>
#include <stdbool.h>

#include "queue.h"

#ifndef CLAB_CRUN_BUFFER_QUEUE_H
#define CLAB_CRUN_BUFFER_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool debug;
  bool close;
  unsigned int capacity;
  Queue *queue;
  pthread_mutexattr_t mutexattr;
  pthread_mutex_t mutex;
  pthread_cond_t prodcond;
  pthread_cond_t conscond;
} BufferQueue;

BufferQueue *bufferq_create(unsigned int capacity);
void bufferq_produce(BufferQueue *bq, void *data);
void *bufferq_consume(BufferQueue *bq);
bool bufferq_can_consume(BufferQueue *bq);
void bufferq_close(BufferQueue *bq);
void bufferq_destroy(BufferQueue *bq, FreeDataFunc freefunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_BUFFER_QUEUE_H
