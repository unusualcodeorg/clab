#include "dslibdemo.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../crun/runtime.h"
#include "../dslib/datastr.h"
#include "../dslib/graph.h"
#include "../dslib/hashmap.h"
#include "../dslib/list.h"
#include "../dslib/path.h"
#include "../dslib/queue.h"
#include "../dslib/stack.h"
#include "../dslib/tree.h"
#include "../dslib/util.h"
#include "../term/console.h"
#include "model.h"

/*--------------------STACK DEMO------------------------ */
int stack_demo(void) {
  printf("\n--------------STACK DEMO----------------\n");
  Stack *stack = stack_create();

  stack_push(stack, new_customer(1, "Janishar Ali 1", 100, true));
  stack_push(stack, new_customer(2, "Janishar Ali 2", 10, true));
  stack_push(stack, new_customer(3, "Janishar Ali 3", 5, true));
  stack_push(stack, new_customer(4, "Janishar Ali 4", 87, true));

  printf("Stack: Size = %zu\n", stack->size);

  char *data = stack_get(stack, 2);
  printf("Stack: Get - Position 2 = %s\n", customer_to_string(data));

  stack_pop(stack, free_data_func);
  printf("Stack: Pop - Size = %zu\n", stack->size);

  Customer *customer = stack_peek(stack);
  printf("Stack: Peek = %s\n", customer_to_string(customer));

  stack_print(stack, customer_to_string);
  stack_destroy(stack, free_data_func);
  printf("--------------STACK DEMO----------------\n");
  return 0;
}
/*--------------------STACK DEMO------------------------ */

/*--------------------STACK CUNCURRENT DEMO------------- */

void push_customer(Stack *stack, char *name, int i) {
  char buff[50];
  snprintf(buff, 50, "%s %d", name, i);
  Customer *customer = new_customer(i, buff, 100 * i, true);
  stack_push(stack, customer);
  printf("  %s\n", customer_to_string(customer));
}

void *s_thread_1_push_function(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);
  Stack *stack = (Stack *)arg;

  for (int i = 1; i < 3; i++) {
    push_customer(stack, "Tom", i);
    usleep(1500000);
  }

  printf("Thread:%lu, Stack: Size = %zu\n", tid, stack->size);
  return NULL;
}

void *s_thread_2_push_function(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);
  Stack *stack = (Stack *)arg;

  for (int i = 1; i < 5; i++) {
    push_customer(stack, "Hardy", i);
    usleep(2000000);
  }

  printf("Thread:%lu, Stack: Size = %zu\n", tid, stack->size);

  Customer *cust = stack_peek(stack);
  printf("Thread:%lu, Stack: Pop = %s\n", tid, customer_to_string(cust));
  stack_pop(stack, free_data_func);
  printf("Thread:%lu, Stack: Size = %zu\n", tid, stack->size);

  return NULL;
}

int stack_concurrent_demo(void) {
  printf("\n---------STACK CUNCURRENT DEMO---------\n");
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);

  pthread_t thread1, thread2;
  Stack *stack = stack_create();

  pthread_create(&thread1, NULL, s_thread_1_push_function, stack);
  pthread_create(&thread2, NULL, s_thread_2_push_function, stack);

  printf("Thread:%lu, Stack: Size = %zu\n", tid, stack->size);
  stack_print(stack, customer_to_string);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Thread ID: End: %lu\n", tid);
  printf("Thread:%lu, Stack: Size = %zu\n", tid, stack->size);

  stack_print(stack, customer_to_string);
  stack_destroy(stack, free_data_func);
  printf("---------STACK CUNCURRENT DEMO---------\n");
  return EXIT_SUCCESS;
}

/*--------------------STACK CUNCURRENT DEMO------------- */

/*--------------------QUEUE DEMO------------------------ */

int queue_demo(void) {
  printf("\n--------------QUEUE DEMO----------------\n");
  Queue *queue = queue_create();

  queue_enqueue(queue, new_customer(1, "Janishar Ali 1", 100, true));
  queue_enqueue(queue, new_customer(2, "Janishar Ali 2", 10, true));
  queue_enqueue(queue, new_customer(3, "Janishar Ali 3", 5, true));
  queue_enqueue(queue, new_customer(4, "Janishar Ali 4", 87, true));

  printf("Queue: Size = %zu\n", queue->size);

  Customer *data = queue_get(queue, 2);
  printf("Queue: Get - Position 2 = %s\n", customer_to_string(data));

  queue_dequeue(queue, free_data_func);
  printf("Queue Dequeue: Size = %zu\n", queue->size);

  Customer *customer = queue_peek(queue);
  printf("Queue: Peek = %s\n", customer_to_string(customer));

  queue_print(queue, customer_to_string);
  printf("--------------QUEUE DEMO----------------\n");
  return 0;
}
/*--------------------QUEUE DEMO------------------------ */

/*--------------------QUEUE CUNCURRENT DEMO------------- */

void queue_customer(Queue *queue, char *name, int i) {
  char buff[50];
  snprintf(buff, 50, "%s %d", name, i);
  Customer *customer = new_customer(i, buff, 100 * i, true);
  queue_enqueue(queue, customer);
  printf("  %s\n", customer_to_string(customer));
}

void *q_thread_1_push_function(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);
  Queue *queue = (Queue *)arg;

  for (int i = 1; i < 3; i++) {
    queue_customer(queue, "Tom", i);
    usleep(1500000);
  }

  printf("Thread:%lu, Queue: Size = %zu\n", tid, queue->size);
  return NULL;
}

void *q_thread_2_push_function(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);
  Queue *queue = (Queue *)arg;

  for (int i = 1; i < 5; i++) {
    queue_customer(queue, "Hardy", i);
    usleep(2000000);
  }

  printf("Thread:%lu, Queue: Size = %zu\n", tid, queue->size);

  Customer *cust = queue_peek(queue);
  printf("Thread:%lu, Queue: Pop = %s\n", tid, customer_to_string(cust));
  queue_dequeue(queue, free_data_func);
  printf("Thread:%lu, Queue: Size = %zu\n", tid, queue->size);

  return NULL;
}

int queue_concurrent_demo(void) {
  printf("\n---------QUEUE CUNCURRENT DEMO---------\n");
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: Create: %lu\n", tid);

  pthread_t thread1, thread2;
  Queue *queue = queue_create();

  pthread_create(&thread1, NULL, q_thread_1_push_function, queue);
  pthread_create(&thread2, NULL, q_thread_2_push_function, queue);

  printf("Thread:%lu, Queue: Size = %zu\n", tid, queue->size);
  queue_print(queue, customer_to_string);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Thread ID: End: %lu\n", tid);
  printf("Thread:%lu, Queue: Size = %zu\n", tid, queue->size);

  queue_print(queue, customer_to_string);
  queue_destroy(queue, free_data_func);
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
 [1]A --> [2]B [6]F,
 [2]B --> [1]A [3]C [5]E,
 [6]F --> [1]A [5]E [7]G,
 [3]C --> [2]B [4]D,
 [5]E --> [2]B [4]D [6]F [8]H,
 [7]G --> [6]F [8]H,
 [4]D --> [3]C [5]E [9]I,
 [8]H --> [5]E [7]G [9]I,
 [9]I --> [4]D [8]H,
]
 */

int graph_demo(void) {
  printf("\n---------------GRAPH DEMO----------------\n");
  Graph *graph = graph_create();
  graph->debug = true;

  size_t id_A = graph_insert(graph, "A", 0);
  size_t id_B = graph_insert(graph, "B", 1, id_A);
  size_t id_C = graph_insert(graph, "C", 1, id_B);
  size_t id_D = graph_insert(graph, "D", 1, id_C);
  size_t id_E = graph_insert(graph, "E", 2, id_B, id_D);
  size_t id_F = graph_insert(graph, "F", 2, id_A, id_E);
  size_t id_G = graph_insert(graph, "G", 1, id_F);
  size_t id_H = graph_insert(graph, "H", 2, id_E, id_G);
  size_t id_I = graph_insert(graph, "I", 2, id_D, id_H);
  (void)id_I;  // suppress unused warning

  graph_print(graph, str_data_to_string);
  graph_destroy(graph, NULL);
  printf("\n---------------GRAPH DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*-----------------------GRAPH DEMO--------------------- */

/*-----------------------GRAPH CONCURRENT DEMO-----------*/

void croutine1(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: croutine1: %lu\n", tid);

  Graph *graph = arg;
  size_t id_A = 1;
  size_t id_B = graph_insert(graph, "B", 1, id_A);

  usleep(1250000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_C = graph_insert(graph, "C", 1, id_B);

  usleep(2250000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_D = graph_insert(graph, "D", 1, id_C);

  usleep(550000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_E = graph_insert(graph, "E", 2, id_B, id_D);

  usleep(1500000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_F = graph_insert(graph, "F", 2, id_A, id_E);

  usleep(100000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_G = graph_insert(graph, "G", 1, id_F);

  usleep(300000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_H = graph_insert(graph, "H", 2, id_E, id_G);

  usleep(1000000);
  printf("Thread ID: croutine1: %lu\n", tid);
  size_t id_I = graph_insert(graph, "I", 2, id_D, id_H);

  (void)id_I;  // suppress unused warning
  usleep(2000000);
}

void croutine2(void *arg) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: croutine2: %lu\n", tid);

  Graph *graph = arg;
  usleep(2000000);
  printf("Thread ID: croutine2: %lu\n", tid);
  char *data = (char *)graph_get(graph, 5);
  printf("Graph found id %d : %s\n", 40, data);

  usleep(2200000);
  printf("Thread ID: croutine2: %lu\n", tid);
  data = (char *)graph_get(graph, 2);
  printf("Graph found id %d : %s\n", 40, data);
}

int graph_concurrent_demo(void) {
  printf("\n---------------GRAPH CONCURRENT DEMO----------------\n");
  Runtime *r1 = runtime_create();
  runtime_debug(r1, "Runtime1");

  Runtime *r2 = runtime_create();
  runtime_debug(r2, "Runtime2");

  unsigned long tid = (unsigned long)pthread_self();
  printf("Thread ID: main: %lu\n", tid);

  Graph *graph = graph_create();
  graph->debug = true;
  graph_insert(graph, "A", 0);

  runtime_exec(r1, croutine1, graph);
  runtime_exec(r2, croutine2, graph);

  runtime_join_destroy(r1);
  runtime_join_destroy(r2);

  printf("Thread ID: main: %lu\n", tid);
  graph_print(graph, str_data_to_string);
  graph_destroy(graph, NULL);
  printf("\n---------------GRAPH CONCURRENT DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*-----------------------GRAPH CONCURRENT DEMO-----------*/

/*------------------------TREE DEMO-------------------- */

int tree_demo(void) {
  printf("\n---------------TREE DEMO----------------\n");
  Tree *tree = tree_create();
  tree->debug = true;

  size_t id_A = tree_insert_root(tree, "A");
  size_t id_B = tree_insert(tree, "B", id_A);
  size_t id_C = tree_insert(tree, "C", id_A);
  size_t id_D = tree_insert(tree, "D", id_A);
  size_t id_E = tree_insert(tree, "E", id_B);
  tree_insert(tree, "F", id_B);
  tree_insert(tree, "G", id_C);
  tree_insert(tree, "H", id_C);
  tree_insert(tree, "I", id_D);
  tree_insert(tree, "J", id_E);
  tree_insert(tree, "K", id_E);
  tree_insert(tree, "L", id_E);
  size_t id_M = tree_insert(tree, "M", id_A);
  tree_insert(tree, "N", id_A);
  tree_insert(tree, "O", id_M);
  tree_insert(tree, "P", id_M);

  tree_print(tree, str_data_to_string);

  char *data = tree_get(tree, id_C);
  printf("\nTree: Get - id %zu = %s\n", id_C, data);

  int maxdepth = tree_max_depth(tree);
  printf("\nTree: Max Depth = %d\n", maxdepth);

  tree_delete(tree, id_C, NULL);
  printf("\nTree: Remove - id %zu = %s", id_D, data);
  tree_print_raw(tree, str_data_to_string);
  tree_print(tree, str_data_to_string);

  printf("\n---------------TREE DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*------------------------TREE DEMO-------------------- */

/*---------------------LINKED LIST DEMO---------------- */

bool list_customer_matcher(void *item, void *match) {
  Customer *customer = (Customer *)item;
  int id = *(int *)match;
  return id == customer->id;
}

int linked_list_demo(void) {
  printf("\n-----------LINKED LIST DEMO-------------\n");

  List *list = list_create();

  int result = list_add_at(list, new_customer(1, "Janishar Ali 1", 100, true), 0);
  printf("List add at 0 when empty result = %d\n", result);
  list_print(list, customer_to_string);

  list_delete_at(list, 0, free_data_func);
  printf("List delete at 0\n");
  list_print(list, customer_to_string);

  list_delete_at(list, 1, free_data_func);
  printf("List delete at 1 when empty\n");
  list_print(list, customer_to_string);

  result = list_add(list, new_customer(2, "Janishar Ali 2", 100, true));
  printf("List add when empty result = %d\n", result);
  list_print(list, customer_to_string);

  result = list_add_at(list, new_customer(3, "Janishar Ali 3", 100, true), 0);
  printf("List add at 0 when not empty result = %d\n", result);
  list_print(list, customer_to_string);

  list_add(list, new_customer(4, "Janishar Ali 4", 100, true));
  list_add(list, new_customer(5, "Janishar Ali 5", 100, true));
  list_add(list, new_customer(6, "Janishar Ali 6", 100, true));

  Customer *customer = new_customer(7, "Janishar Ali 7", 100, true);
  list_add(list, customer);

  list_add(list, new_customer(8, "Janishar Ali 8", 100, true));
  list_print(list, customer_to_string);

  Customer *deleted = list_delete_at(list, 2, NULL);
  char *str = customer_to_string(deleted);
  printf("List delete at 2 result = %s\n", deleted == NULL ? "Not found" : str);
  free(str);
  free(deleted);
  str = NULL;
  deleted = NULL;
  list_print(list, customer_to_string);

  list_delete_at(list, 0, free_data_func);
  printf("List delete at 0\n");
  list_print(list, customer_to_string);

  list_delete_at(list, list->size - 1, free_data_func);
  printf("List delete at %zu\n", list->size);
  list_print(list, customer_to_string);

  Customer *item = list_get_at(list, 2);
  str = customer_to_string(item);
  printf("List get at 2 result = %s\n", item == NULL ? "Not found" : str);
	free(str);
  str = NULL;

  item = list_get_at(list, 20);
  printf("List get at 20 result = %s\n", item == NULL ? "Not found" : "Some value");

  result = list_index_of(list, customer, list_customer_matcher);
  str = customer_to_string(customer);
  printf("List index of %s = %d\n", str, result);
	free(str);
  str = NULL;

  list_destroy(list, free_data_func);

  printf("\n-----------LINKED LIST DEMO-------------\n");
  return EXIT_SUCCESS;
}
/*---------------------LINKED LIST DEMO---------------- */

/*-------------------HASHMAP DEMO-----------------------*/

int hashmap_demo(void) {
  printf("\n------------HASHMAP DEMO------------------\n");
  HashMap *map = hashmap_create(10);
  hashmap_put(map, "A", "N1");
  hashmap_put(map, "B", "N2");
  hashmap_put(map, "C", "N3");
  hashmap_put(map, "D", "N4");
  hashmap_put(map, "E", "N5");
  hashmap_put(map, "F", "N6");
  hashmap_put(map, "F", "N7");
  hashmap_put(map, "F", "N8");
  hashmap_put(map, "G", "N9");
  hashmap_print(map, str_data_to_string);

  printf("\nHashMap Get %s - %s\n\n", "E", (char *)hashmap_get(map, "E"));
  hashmap_delete(map, "F", NULL);
  hashmap_delete(map, "F", NULL);
  hashmap_print(map, str_data_to_string);

  hashmap_destroy(map, NULL);
  printf("\n------------HASHMAP DEMO------------------\n");
  return EXIT_SUCCESS;
}

/*-------------------HASHMAP DEMO----------------------*/
