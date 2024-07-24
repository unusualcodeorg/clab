#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue *queue_new()
{
	Queue *queue = malloc(sizeof(Queue));
	queue->start = NULL;
	queue->end = NULL;
	queue->size = 0;
	return queue;
}

void queue_enqueue(Queue *queue, void *data)
{
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
}

void queue_dequeue(Queue *queue)
{
	if (queue->start == NULL)
		return;

	QueueNode *node = queue->start;
	queue->start = node->next;
	queue->size--;
	free(node->data);
	free(node);
}

void *queue_peek(Queue *queue)
{
	if (queue->start == NULL)
		return NULL;

	return queue->start->data;
}

void queue_print(Queue *queue, fn_to_string to_string)
{
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
}

void queue_destroy(Queue *queue)
{
	QueueNode *start = queue->start;
	while (start != NULL)
	{
		QueueNode *next = start->next;
		free(start->data);
		free(start);
		start = next;
	}

	free(queue);
}