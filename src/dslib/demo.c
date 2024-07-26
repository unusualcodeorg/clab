#include "demo.h"
#include "stack.h"
#include "queue.h"
#include "model.h"
#include "graph.h"
#include "tree.h"
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

	char *data = stack_get(stack, 2);
	printf("Stack: Get - Position 2 = %s\n", data);

	stack_pop(stack);
	printf("Stack: Pop - Size = %d\n", stack->size);

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

	char *data = queue_get(queue, 2);
	printf("Queue: Get - Position 2 = %s\n", data);

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
 [0]A --> [1]B [5]F,
 [5]F --> [0]A [4]E [6]G,
 [6]G --> [5]F [7]H,
 [7]H --> [4]E [6]G [8]I,
 [8]I --> [3]D [7]H,
 [3]D --> [2]C [4]E [8]I,
 [4]E --> [1]B [3]D [5]F [7]H,
 [1]B --> [0]A [2]C [4]E,
 [2]C --> [1]B [3]D,
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
	Graph *graph = graph_create(false);
	graph->debug = true;

	unsigned int id_A = graph_add(graph, "A", 0);
	unsigned int id_B = graph_add(graph, "B", 1, id_A);
	unsigned int id_C = graph_add(graph, "C", 1, id_B);
	unsigned int id_D = graph_add(graph, "D", 1, id_C);
	unsigned int id_E = graph_add(graph, "E", 2, id_B, id_D);
	unsigned int id_F = graph_add(graph, "F", 2, id_A, id_E);
	unsigned int id_G = graph_add(graph, "G", 1, id_F);
	unsigned int id_H = graph_add(graph, "H", 2, id_E, id_G);
	unsigned int id_I = graph_add(graph, "I", 2, id_D, id_H);
	(void)id_I; // suppress unused warning

	graph_print(graph, graph_data_to_string);
	graph_destroy(graph);
	return EXIT_SUCCESS;
}

/*-----------------------GRAPH DEMO--------------------- */

/*-----------------------GRAPH 2D ARR DEMO-------------- */
char *graph_maze_data_to_string(void *arg)
{
	char data = *(char *)arg;
	char *buffer = malloc(50);
	snprintf(buffer, 50, "%c", data);
	return buffer;
}

int graph_2d_arr_demo(void)
{
	const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";

	int rows = 7;
	int cols = 9;
	char **arr = malloc(rows * sizeof(char *));
	for (int i = 0; i < rows; i++)
		arr[i] = malloc(cols * sizeof(char));

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			char data = maze[i * cols + j];
			arr[i][j] = data;
			printf("%c", data);
		}
		printf("\n");
	}

	Graph *graph = graph_from_2d_arr(arr, rows, cols, false);
	graph->debug = true;
	graph_print(graph, graph_maze_data_to_string);

	char *data = (char *)graph_get(graph, 40);
	printf("Graph found id %d : %c\n", 40, *data);

	data = (char *)graph_get(graph, 24);
	printf("Graph found id %d : %c\n", 24, *data);

	graph_destroy(graph);
	free(arr);
	return EXIT_SUCCESS;
}
/*-----------------------GRAPH 2D ARR DEMO-------------- */

/*------------------------TREE DEMO-------------------- */
char *tree_data_to_string(void *arg)
{
	char data = *(char *)arg;
	char *buffer = malloc(50);
	snprintf(buffer, 50, "%c", data);
	return buffer;
}
int tree_demo(void)
{
	Tree *tree = tree_create(true);
	tree->debug = true;

	unsigned int id_A = tree_add_root(tree, "A");
	unsigned int id_B = tree_add(tree, "B", id_A);
	unsigned int id_C = tree_add(tree, "C", id_A);
	unsigned int id_D = tree_add(tree, "D", id_A);
	unsigned int id_E = tree_add(tree, "E", id_B);
	tree_add(tree, "F", id_B);
	tree_add(tree, "G", id_C);
	tree_add(tree, "H", id_C);
	tree_add(tree, "I", id_D);
	tree_add(tree, "J", id_E);
	tree_add(tree, "K", id_E);
	tree_add(tree, "L", id_E);

	tree_print(tree, tree_data_to_string);

	char *data = tree_get(tree, id_C);
	printf("\nTree: Get - id %d = %s\n", id_C, data);

	tree_remove(tree, id_C);
	printf("\nTree: Remove - id %d = %s", id_D, data);
	tree_print(tree, tree_data_to_string);

	return EXIT_FAILURE;
}
/*------------------------TREE DEMO-------------------- */
