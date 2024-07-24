#include "demo.h"
#include "stack.h"
#include "queue.h"
#include "model.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*--------------------STACK DEMO------------------------ */
int stack_demo(void)
{
	Stack *stack = stack_create();

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

	return 0;
}
/*--------------------STACK DEMO------------------------ */

/*--------------------STACK CUNCURRENT DEMO------------- */

void push_customer(Stack *stack, char i)
{
	char name[] = "Ali";
	char result[strlen(name) + 1];
	sprintf("%s %d", result, i);
	Customer *customer = new_customer(result, 100 * i, true);
	stack_push(stack, customer);
	printf("%s\n", customer_to_string(customer));
}

void *s_thread_1_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: %lu\n", tid);
	Stack *stack = (Stack *)arg;

	for (char i = 1; i < 4; i++)
	{
		push_customer(stack, i);
		usleep(1500000);
	}

	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);
	return NULL;
}

void *s_thread_2_push_function(void *arg)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: %lu\n", tid);
	Stack *stack = (Stack *)arg;

	for (char i = 1; i < 2; i++)
	{
		push_customer(stack, i);
		usleep(2000000);
	}

	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);

	Customer *c = stack_peek(stack);
	printf("Thread:%lu, Stack: Pop = %s\n", tid, customer_to_string(c));
	stack_pop(stack);
	printf("Thread:%lu, Stack: Size = %d\n", tid, stack->size);

	return NULL;
}

int stack_concurrent_demo(void)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("Thread ID: Start: %lu\n", tid);

	pthread_t thread1, thread2;
	Stack *stack = stack_create();

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

	return EXIT_SUCCESS;
}

/*--------------------STACK CUNCURRENT DEMO------------- */

/*--------------------QUEUE DEMO------------------------ */

int queue_demo(void)
{
	Queue *queue = queue_create();

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

	return 0;
}
/*--------------------QUEUE DEMO------------------------ */
