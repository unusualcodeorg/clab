#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char *(*fn_to_string)(void *);

	typedef struct QueueNode
	{
		void *data;
		struct QueueNode *next;
	} QueueNode;

	typedef struct
	{
		QueueNode *start;
		QueueNode *end;
		uint32_t size;
		bool autofree;
		pthread_rwlock_t rwlock;
	} Queue;

	Queue *queue_create(bool autofree);
	void queue_enqueue(Queue *queue, void *data);
	void *queue_dequeue(Queue *queue);
	void *queue_peek(Queue *queue);
	void queue_print(Queue *queue, fn_to_string to_string);
	void queue_destroy(Queue *queue);

#ifdef __cplusplus
}
#endif

#endif // QUEUE_H
