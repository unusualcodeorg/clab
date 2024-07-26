#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

Queue *queue_create(bool autofree)
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
	queue->autofree = autofree;
	pthread_mutexattr_init(&queue->mutexattr);
	pthread_mutexattr_settype(&queue->mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&queue->mutex, &queue->mutexattr);
	return queue;
}

void queue_enqueue(Queue *queue, void *data)
{
	pthread_mutex_lock(&queue->mutex);
	QueueNode *node = malloc(sizeof(QueueNode));
	node->data = data;
	node->next = NULL;

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
	pthread_mutex_unlock(&queue->mutex);
}

void *queue_dequeue(Queue *queue)
{
	pthread_mutex_lock(&queue->mutex);
	void *data = NULL;
	if (queue->start != NULL)
	{
		QueueNode *node = queue->start;
		queue->start = node->next;
		queue->size--;
		if (queue->autofree == true)
			free(node->data);
		else
			data = node->data;
		free(node);
	}
	pthread_mutex_unlock(&queue->mutex);
	return data;
}

void *queue_peek(Queue *queue)
{
	pthread_mutex_lock(&queue->mutex);
	void *data = NULL;
	if (queue->start != NULL)
		data = queue->start->data;

	pthread_mutex_unlock(&queue->mutex);
	return data;
}

void *queue_get(Queue *queue, unsigned int position)
{
	if (position >= queue->size)
		return NULL;

	pthread_mutex_lock(&queue->mutex);
	QueueNode *node = queue->start;
	unsigned int counter = 1;

	while (node != NULL)
	{
		if (counter > position)
			break;
		node = node->next;
		counter++;
	}

	pthread_mutex_unlock(&queue->mutex);
	return node != NULL ? node->data : NULL;
}

void queue_print(Queue *queue, DataToString tostring)
{
	pthread_mutex_lock(&queue->mutex);
	printf("Queue: [\n");
	QueueNode *start = queue->start;
	while (start != NULL)
	{
		char *str = tostring(start->data);
		printf("  %s", str);
		free(str);

		QueueNode *next = start->next;
		start = next;
		if (start != NULL)
			printf(",\n");
	}
	printf("\n]\n");
	pthread_mutex_unlock(&queue->mutex);
}

void queue_destroy(Queue *queue)
{
	pthread_mutex_lock(&queue->mutex);
	QueueNode *start = queue->start;
	while (start != NULL)
	{
		QueueNode *next = start->next;
		if (queue->autofree == true)
			free(start->data);
		free(start);
		start = next;
	}
	pthread_mutex_unlock(&queue->mutex);
	pthread_mutex_destroy(&queue->mutex);
	pthread_mutexattr_destroy(&queue->mutexattr);
	free(queue);
}
