#include "queue.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Queue *queue_create(void) {
  Queue *queue = malloc(sizeof(Queue));
  if (queue == NULL) {
    perror("failed to allocate memory for queue");
    exit(EXIT_FAILURE);
  }
  queue->start = NULL;
  queue->end = NULL;
  queue->size = 0;
  pthread_rwlock_init(&queue->rwlock, NULL);
  return queue;
}

void queue_enqueue(Queue *queue, void *data) {
  pthread_rwlock_wrlock(&queue->rwlock);
  QueueNode *node = malloc(sizeof(QueueNode));
  node->data = data;
  node->next = NULL;

  if (queue->start == NULL) {
    queue->start = node;
  } else if (queue->end == NULL) {
    queue->start->next = node;
    queue->end = node;
  } else {
    queue->end->next = node;
    queue->end = node;
  }

  queue->size++;
  pthread_rwlock_unlock(&queue->rwlock);
}

void *queue_dequeue(Queue *queue, FreeDataFunc freedatafunc) {
  pthread_rwlock_wrlock(&queue->rwlock);
  void *data = NULL;
  if (queue->start != NULL) {
    QueueNode *node = queue->start;
    queue->start = node->next;
    queue->size--;

    if (queue->start == queue->end) queue->end = NULL;

    if (freedatafunc != NULL)
      freedatafunc(node->data);
    else
      data = node->data;
    free(node);
  }
  pthread_rwlock_unlock(&queue->rwlock);
  return data;
}

void *queue_peek(Queue *queue) {
  pthread_rwlock_rdlock(&queue->rwlock);
  void *data = NULL;
  if (queue->start != NULL) data = queue->start->data;
  pthread_rwlock_unlock(&queue->rwlock);
  return data;
}

void *queue_get(Queue *queue, unsigned int position) {
  if (position >= queue->size) return NULL;

  pthread_rwlock_rdlock(&queue->rwlock);
  QueueNode *node = queue->start;
  unsigned int counter = 1;

  while (node) {
    if (counter > position) break;
    node = node->next;
    counter++;
  }

  pthread_rwlock_unlock(&queue->rwlock);
  return node != NULL ? node->data : NULL;
}

void queue_print(Queue *queue, DataToString tostring) {
  pthread_rwlock_rdlock(&queue->rwlock);
  printf("Queue: [\n");
  QueueNode *start = queue->start;
  while (start) {
    char *str = tostring(start->data);
    printf("  %s", str);
    free(str);

    start = start->next;
    if (start != NULL) printf(",\n");
  }
  printf("\n]\n");
  pthread_rwlock_unlock(&queue->rwlock);
}

void queue_destroy(Queue *queue, FreeDataFunc freedatafunc) {
  pthread_rwlock_trywrlock(&queue->rwlock);

  QueueNode *start = queue->start;
  while (start) {
    QueueNode *next = start->next;
    if (freedatafunc != NULL) freedatafunc(start->data);
    free(start);
    start = next;
  }

  pthread_rwlock_unlock(&queue->rwlock);
  pthread_rwlock_destroy(&queue->rwlock);
  free(queue);
}
