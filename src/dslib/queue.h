#include <pthread.h>
#include <stdbool.h>

#ifndef DS_LIB_QUEUE_H
#define DS_LIB_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char *(*DataToString)(void *);

	typedef struct QueueNode
	{
		void *data;
		struct QueueNode *next;
	} QueueNode;

	typedef struct
	{
		QueueNode *start;
		QueueNode *end;
		unsigned int size;
		bool autofree; // free data on pop
		pthread_rwlock_t rwlock;
	} Queue;

	Queue *queue_create(bool autofree);
	void queue_enqueue(Queue *queue, void *data);
	void *queue_dequeue(Queue *queue);
	void *queue_peek(Queue *queue);
	void queue_print(Queue *queue, DataToString tostring);
	void queue_destroy(Queue *queue);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_QUEUE_H
