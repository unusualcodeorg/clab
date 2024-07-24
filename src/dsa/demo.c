#include "demo.h"
#include "stack.h"
#include "queue.h"
#include "model.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

int stack_demo()
{
	Stack *stack = stack_new();

	stack_push(stack, new_customer("Janishar Ali 1", 100, true));
	stack_push(stack, new_customer("Janishar Ali 2", 10, true));
	stack_push(stack, new_customer("Janishar Ali 3", 5, true));
	stack_push(stack, new_customer("Janishar Ali 4", 87, true));

	printf("Stack: Size = %d\n", stack->size);

	stack_pop(stack);

	printf("Stack Pop: Size = %d\n", stack->size);

	Customer *customer = stack_peek(stack);
	printf("Stack: Peek = {Name: %s, Salary: %d, Verified: %s}\n",
				 customer->name, customer->salary,
				 customer->verified ? "Yes" : "No");

	stack_print(stack, customer_to_string);

	return 0;
}

int queue_demo()
{
	Queue *queue = queue_new();

	queue_enqueue(queue, new_customer("Janishar Ali 1", 100, true));
	queue_enqueue(queue, new_customer("Janishar Ali 2", 10, true));
	queue_enqueue(queue, new_customer("Janishar Ali 3", 5, true));
	queue_enqueue(queue, new_customer("Janishar Ali 4", 87, true));

	printf("Queue: Size = %d\n", queue->size);

	queue_dequeue(queue);

	printf("Queue Dequeue: Size = %d\n", queue->size);

	Customer *customer = queue_peek(queue);
	printf("Queue: Peek = {Name: %s, Salary: %d, Verified: %s}\n",
				 customer->name, customer->salary,
				 customer->verified ? "Yes" : "No");

	queue_print(queue, customer_to_string);

	return 0;
}