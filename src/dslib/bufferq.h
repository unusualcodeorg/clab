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
  bool writerclosed;
  bool readerclosed;
  size_t capacity;
  Queue *queue;
  pthread_mutexattr_t mutexattr;
  pthread_mutex_t mutex;
  pthread_cond_t writecond;
  pthread_cond_t readcond;
} BufferQueue;

BufferQueue *bufferq_create(size_t capacity);
void bufferq_write(BufferQueue *bq, void *data);
void *bufferq_read(BufferQueue *bq);
bool bufferq_can_read(BufferQueue *bq);
void bufferq_close_writer(BufferQueue *bq);
void bufferq_destroy(BufferQueue *bq, FreeDataFunc freefunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_BUFFER_QUEUE_H
