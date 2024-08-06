#include "bufferq.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

BufferQueue *bufferq_create(size_t capacity) {
  BufferQueue *bq = (BufferQueue *)malloc(sizeof(BufferQueue));
  bq->debug = false;
  bq->writerclosed = false;
  bq->readerclosed = false;
  bq->capacity = capacity;
  bq->queue = queue_create();

  pthread_mutexattr_init(&bq->mutexattr);
  pthread_mutexattr_settype(&bq->mutexattr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&bq->mutex, &bq->mutexattr);

  pthread_cond_init(&bq->writecond, NULL);
  pthread_cond_init(&bq->readcond, NULL);
  return bq;
}

void bufferq_write(BufferQueue *bq, void *data) {
  pthread_mutex_lock(&bq->mutex);

  if (bq->queue->size == bq->capacity) {
    if (bq->debug == true) printf("BufferQueue: writer pause.\n");
    pthread_cond_wait(&bq->writecond, &bq->mutex);  // atomically unlock mutex
    if (bq->debug == true) printf("BufferQueue: writer resume.\n");
  }

  if (bq->writerclosed == true) {
    if (bq->debug == true) printf("BufferQueue: writer close.\n");
    pthread_mutex_unlock(&bq->mutex);  // must release after condition
    return;
  }

  if (bq->debug == true) printf("BufferQueue: writer write.\n");
  queue_enqueue(bq->queue, data);
  pthread_cond_signal(&bq->readcond);

  pthread_mutex_unlock(&bq->mutex);  // must release after condition
}

void *bufferq_read(BufferQueue *bq) {
  pthread_mutex_lock(&bq->mutex);

  if (bq->queue->size == 0) {
    if (bq->writerclosed == true) {
      bq->readerclosed = true;
      if (bq->debug == true) printf("BufferQueue: reader close read.\n");
    } else {
      if (bq->debug == true) printf("BufferQueue: reader pause.\n");
      pthread_cond_wait(&bq->readcond, &bq->mutex);  // atomically unlock mutex
      if (bq->debug == true) printf("BufferQueue: reader resume.\n");
    }
  }

  if (bq->readerclosed == true) {
    if (bq->debug == true) printf("BufferQueue: reader close.\n");
    pthread_cond_signal(&bq->readcond); // awake the other readers
    pthread_mutex_unlock(&bq->mutex);  // must release after condition
    return NULL;
  }

  if (bq->debug == true) printf("BufferQueue: reader read.\n");

  void *data = queue_dequeue(bq->queue, NULL);  // must release after condition
  pthread_cond_signal(&bq->writecond);

  pthread_mutex_unlock(&bq->mutex);
  return data;
}

bool bufferq_can_read(BufferQueue *bq) { return !bq->readerclosed; }

void bufferq_close_writer(BufferQueue *bq) {
  pthread_mutex_lock(&bq->mutex);
  bq->writerclosed = true;
  if (bq->debug == true) printf("BufferQueue: writer close write.\n");
  pthread_cond_signal(&bq->writecond);
  pthread_cond_signal(&bq->readcond);
  pthread_mutex_unlock(&bq->mutex);
}

void bufferq_destroy(BufferQueue *bq, FreeDataFunc freedatafunc) {
  bufferq_close_writer(bq);
  queue_destroy(bq->queue, freedatafunc);

  pthread_mutex_destroy(&bq->mutex);
  pthread_mutexattr_destroy(&bq->mutexattr);
  pthread_cond_destroy(&bq->writecond);
  pthread_cond_destroy(&bq->readcond);

  if (bq->debug == true) printf("BufferQueue: has destroyed.\n");
  free(bq);
}
