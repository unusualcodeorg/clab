#include "demo.h"
#include "stack.h"
#include "queue.h"
#include "model.h"
#include "graph.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*--------------------STACK DEMO------------------------ */
int stack_demo(void)
{
	printf("\n--------------STACK DEMO----------------\n");
	Stack *stack = stack_create(true);

	stack_push(stack, new_customer("Janishar Ali 1", 100, true));
	stack_push(stack, new_customer("Janishar Ali 2", 10, true));
	stack_push(stack, new_customer("Janishar Ali 3", 5, true));
	stack_push(stack, new_customer("Janishar Ali 4", 87, true));

	printf("Stack: Size = %d\n", stack->size);

	stack_pop(stack);
	printf("Stack Pop: Size = %d\n", stack->size);

	Customer *customer = stack_peek(stack);
	printf("Stack: Peek = %s\n", customer_to_string(customer));

	stack_print(stack, customer_to_string);
	printf("--------------STACK DEMO----------------\n");
	return 0;
}
/*--------------------STACK DEMO------------------------ */

/*--------------------STACK CUNCURRENT DEMO------------- */

void push_customer(Stack *stack, char *name, char i)
{
	char buff[50];
	snprintf(buff, 50, "%s %d", name, i);
	Customer *customer = new_customer(buff, 100 * i, true);
	stack_push(stack, customer);
	printf("  %s\n", customer_to_string(customer));
}

void *s_thread_1_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);
	Stack *stack = (Stack *)arg;

	for (char i = 1; i < 3; i++)
	{
		push_customer(stack, "Tom", i);
		usleep(1500000);
	}

	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);
	return NULL;
}

void *s_thread_2_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);
	Stack *stack = (Stack *)arg;

	for (char i = 1; i < 5; i++)
	{
		push_customer(stack, "Hardy", i);
		usleep(2000000);
	}

	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);

	Customer *cust = stack_peek(stack);
	printf("Thread:%lu, Stack: Pop = %s\n", tid, customer_to_string(cust));
	stack_pop(stack);
	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);

	return NULL;
}

int stack_concurrent_demo(void)
{
	printf("\n---------STACK CUNCURRENT DEMO---------\n");
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);

	pthread_t thread1, thread2;
	Stack *stack = stack_create(true);

	pthread_create(&thread1, NULL, s_thread_1_push_function, stack);
	pthread_create(&thread2, NULL, s_thread_2_push_function, stack);

	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);
	stack_print(stack, customer_to_string);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Thread ID: End: %lu\n", tid);
	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);

	stack_print(stack, customer_to_string);
	stack_destroy(stack);
	printf("---------STACK CUNCURRENT DEMO---------\n");
	return EXIT_SUCCESS;
}

/*--------------------STACK CUNCURRENT DEMO------------- */

/*--------------------QUEUE DEMO------------------------ */

int queue_demo(void)
{
	printf("\n--------------QUEUE DEMO----------------\n");
	Queue *queue = queue_create(true);

	queue_enqueue(queue, new_customer("Janishar Ali 1", 100, true));
	queue_enqueue(queue, new_customer("Janishar Ali 2", 10, true));
	queue_enqueue(queue, new_customer("Janishar Ali 3", 5, true));
	queue_enqueue(queue, new_customer("Janishar Ali 4", 87, true));

	printf("Queue: Size = %d\n", queue->size);

	queue_dequeue(queue);
	printf("Queue Dequeue: Size = %d\n", queue->size);

	Customer *customer = queue_peek(queue);
	printf("Queue: Peek = %s\n", customer_to_string(customer));

	queue_print(queue, customer_to_string);
	printf("--------------QUEUE DEMO----------------\n");
	return 0;
}
/*--------------------QUEUE DEMO------------------------ */

/*--------------------QUEUE CUNCURRENT DEMO------------- */

void queue_customer(Queue *queue, char *name, char i)
{
	char buff[50];
	snprintf(buff, 50, "%s %d", name, i);
	Customer *customer = new_customer(buff, 100 * i, true);
	queue_enqueue(queue, customer);
	printf("  %s\n", customer_to_string(customer));
}

void *q_thread_1_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);
	Queue *queue = (Queue *)arg;

	for (char i = 1; i < 3; i++)
	{
		queue_customer(queue, "Tom", i);
		usleep(1500000);
	}

	printf("Thread:%lu, Queue: Size = %d\n", tid, queue->size);
	return NULL;
}

void *q_thread_2_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);
	Queue *queue = (Queue *)arg;

	for (char i = 1; i < 5; i++)
	{
		queue_customer(queue, "Hardy", i);
		usleep(2000000);
	}

	printf("Thread:%lu, Queue: Size = %d\n", tid, queue->size);

	Customer *cust = queue_peek(queue);
	printf("Thread:%lu, Queue: Pop = %s\n", tid, customer_to_string(cust));
	queue_dequeue(queue);
	printf("Thread:%lu, Queue: Size = %d\n", tid, queue->size);

	return NULL;
}

int queue_concurrent_demo(void)
{
	printf("\n---------QUEUE CUNCURRENT DEMO---------\n");
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Create: %lu\n", tid);

	pthread_t thread1, thread2;
	Queue *queue = queue_create(true);

	pthread_create(&thread1, NULL, q_thread_1_push_function, queue);
	pthread_create(&thread2, NULL, q_thread_2_push_function, queue);

	printf("Thread:%lu, Queue: Size = %d\n", tid, queue->size);
	queue_print(queue, customer_to_string);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Thread ID: End: %lu\n", tid);
	printf("Thread:%lu, Queue: Size = %d\n", tid, queue->size);

	queue_print(queue, customer_to_string);
	queue_destroy(queue);
	printf("---------QUEUE CUNCURRENT DEMO---------\n");
	return EXIT_SUCCESS;
}

/*--------------------QUEUE CUNCURRENT DEMO------------- */

/*-----------------------GRAPH DEMO--------------------- */

/*
[0]A----[1]B----[2]C
	 |       |       |
	 |       |       |
[5]F----[4]E----[3]D
	 |       |       |
	 |       |       |
[6]G----[7]H----[8]I

Graph[
 [8]:I - [3]:D, [7]:H,
 [7]:H - [4]:E, [6]:G, [8]:I,
 [6]:G - [5]:F, [7]:H,
 [5]:F - [0]:A, [4]:E, [6]:G,
 [4]:E - [1]:B, [3]:D, [5]:F, [7]:H,
 [3]:D - [2]:C, [4]:E, [8]:I,
 [2]:C - [1]:B, [3]:D,
 [1]:B - [0]:A, [2]:C, [4]:E,
 [0]:A - [1]:B, [5]:F,
]
 */

char *graph_data_to_string(void *arg)
{
	char *data = (char *)arg;
	char *buffer = malloc(100);
	snprintf(buffer, 100, "%s", data);
	return buffer;
}

int graph_demo(void)
{
	Graph *graph = graph_create(true, true);

	graph_add(graph, "A", NULL, 0);
	graph_add(graph, "B", (unsigned int[]){0}, 1);
	graph_add(graph, "C", (unsigned int[]){1}, 1);
	graph_add(graph, "D", (unsigned int[]){2}, 1);
	graph_add(graph, "E", (unsigned int[]){1, 3}, 2);
	graph_add(graph, "F", (unsigned int[]){0, 4}, 2);
	graph_add(graph, "G", (unsigned int[]){5}, 1);
	graph_add(graph, "H", (unsigned int[]){4, 6}, 2);
	graph_add(graph, "I", (unsigned int[]){3, 7}, 2);

	graph_print(graph, graph_data_to_string);
	return EXIT_SUCCESS;
}

/*-----------------------GRAPH DEMO--------------------- */
