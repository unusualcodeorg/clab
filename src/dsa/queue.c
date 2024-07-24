#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

Queue *queue_new()
{
	Queue *queue = malloc(sizeof(Queue));
	if (queue == NULL)
	{
		perror("failed to allocate memory for queue");
		exit(EXIT_FAILURE);
	}
	queue->start = NULL;
	queue->end = NULL;
	queue->size = 0;
	pthread_rwlock_init(&queue->rwlock, NULL);
	return queue;
}

void queue_enqueue(Queue *queue, void *data)
{
	pthread_rwlock_wrlock(&queue->rwlock);
	QueueNode *node = malloc(sizeof(QueueNode));
	node->data = data;

	if (queue->start == NULL)
	{
		queue->start = node;
	}
	else if (queue->end == NULL)
	{
		queue->start->next = node;
		queue->end = node;
	}
	else
	{
		queue->end->next = node;
		queue->end = node;
	}

	queue->size++;
	pthread_rwlock_unlock(&queue->rwlock);
}

void queue_dequeue(Queue *queue)
{
	pthread_rwlock_wrlock(&queue->rwlock);
	if (queue->start != NULL)
	{
		QueueNode *node = queue->start;
		queue->start = node->next;
		queue->size--;
		free(node->data);
		free(node);
	}
	pthread_rwlock_unlock(&queue->rwlock);
}

void *queue_peek(Queue *queue)
{
	pthread_rwlock_rdlock(&queue->rwlock);
	void *data = NULL;
	if (queue->start != NULL)
	{
		data = queue->start->data;
	}

	pthread_rwlock_unlock(&queue->rwlock);
	return data;
}

void queue_print(Queue *queue, fn_to_string to_string)
{
	pthread_rwlock_rdlock(&queue->rwlock);
	printf("Queue: [\n");
	QueueNode *start = queue->start;
	while (start != NULL)
	{
		char *str = to_string(start->data);
		printf("  %s", str);
		free(str);

		QueueNode *next = start->next;
		start = next;
		if (start != NULL)
			printf(",\n");
	}
	printf("\n]\n");
	pthread_rwlock_unlock(&queue->rwlock);
}

void queue_destroy(Queue *queue)
{
	pthread_rwlock_wrlock(&queue->rwlock);
	QueueNode *start = queue->start;
	while (start != NULL)
	{
		QueueNode *next = start->next;
		free(start->data);
		free(start);
		start = next;
	}
	pthread_rwlock_unlock(&queue->rwlock);
	pthread_rwlock_destroy(&queue->rwlock);
	free(queue);
}