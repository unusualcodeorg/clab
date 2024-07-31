#include "bufferq.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

BufferQueue *bufferq_create(unsigned int capacity) {
  BufferQueue *bq = (BufferQueue *)malloc(sizeof(BufferQueue));
  bq->debug = false;
  bq->close = false;
  bq->capacity = capacity;
  bq->queue = queue_create();

  pthread_mutexattr_init(&bq->mutexattr);
  pthread_mutexattr_settype(&bq->mutexattr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&bq->mutex, &bq->mutexattr);

  pthread_cond_init(&bq->prodcond, NULL);
  pthread_cond_init(&bq->conscond, NULL);
  return bq;
}

void bufferq_produce(BufferQueue *bq, void *data) {
  pthread_mutex_lock(&bq->mutex);

  if (bq->queue->size == bq->capacity) {
    if (bq->debug == true) printf("BufferQueue: producer pause.\n");
    pthread_cond_wait(&bq->prodcond, &bq->mutex);  // atomically unlock mutex
    if (bq->debug == true) printf("BufferQueue: producer resume.\n");
  }

  if (bq->close == true) {
    if (bq->debug == true) printf("BufferQueue: producer close.\n");
    pthread_mutex_unlock(&bq->mutex);  // must release after condition
    return;
  }

  if (bq->debug == true) printf("BufferQueue: producer produce.\n");
  queue_enqueue(bq->queue, data);
  pthread_cond_signal(&bq->conscond);

  pthread_mutex_unlock(&bq->mutex);  // must release after condition
}

void *bufferq_consume(BufferQueue *bq) {
  pthread_mutex_lock(&bq->mutex);

  if (bq->queue->size == 0) {
    if (bq->debug == true) printf("BufferQueue: consumer pause.\n");
    pthread_cond_wait(&bq->conscond, &bq->mutex);  // atomically unlock mutex
    if (bq->debug == true) printf("BufferQueue: consumer resume.\n");
  }

  if (bq->close == true) {
    if (bq->debug == true) printf("BufferQueue: consumer close.\n");
    pthread_mutex_unlock(&bq->mutex);  // must release after condition
    return NULL;
  }

  if (bq->debug == true) printf("BufferQueue: consumer consume.\n");
  
	void *data = queue_dequeue(bq->queue, NULL);  // must release after condition
  pthread_cond_signal(&bq->prodcond);
  
	pthread_mutex_unlock(&bq->mutex);
  return data;
}

bool bufferq_can_consume(BufferQueue *bq) { return !bq->close; }

void bufferq_close(BufferQueue *bq) {
  pthread_mutex_lock(&bq->mutex);
  bq->close = true;
  pthread_cond_signal(&bq->prodcond);
  pthread_cond_signal(&bq->conscond);
  pthread_mutex_unlock(&bq->mutex);
}

void bufferq_destroy(BufferQueue *bq, FreeDataFunc freedatafunc) {
  bufferq_close(bq);
  queue_destroy(bq->queue, freedatafunc);

  pthread_mutex_destroy(&bq->mutex);
  pthread_mutexattr_destroy(&bq->mutexattr);
  pthread_cond_destroy(&bq->prodcond);
  pthread_cond_destroy(&bq->conscond);

  if (bq->debug == true) printf("BufferQueue: has destroyed.\n");
  free(bq);
}
