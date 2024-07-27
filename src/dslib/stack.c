#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

Stack *stack_create(bool autofree)
{
	Stack *stack = malloc(sizeof(Stack));
	if (stack == NULL)
	{
		perror("failed to allocate memory for stack");
		exit(EXIT_FAILURE);
	}
	stack->top = NULL;
	stack->size = 0;
	stack->autofree = autofree;
	pthread_rwlock_init(&stack->rwlock, NULL);
	return stack;
}

void stack_push(Stack *stack, void *data)
{
	pthread_rwlock_wrlock(&stack->rwlock);
	StackNode *node = malloc(sizeof(StackNode));
	node->data = data;
	node->next = stack->top;
	stack->top = node;
	stack->size++;
	pthread_rwlock_unlock(&stack->rwlock);
}

void *stack_pop(Stack *stack)
{
	pthread_rwlock_wrlock(&stack->rwlock);
	void *data = NULL;
	if (stack->top != NULL)
	{
		StackNode *node = stack->top;
		stack->top = stack->top->next;
		stack->size--;
		if (stack->autofree == true)
			free(node->data);
		else
			data = node->data;
		free(node);
	}
	pthread_rwlock_unlock(&stack->rwlock);
	return data;
}

void *stack_peek(Stack *stack)
{
	pthread_rwlock_rdlock(&stack->rwlock);
	void *data = NULL;
	if (stack->top != NULL)
		data = stack->top->data;
	pthread_rwlock_unlock(&stack->rwlock);
	return data;
}

void *stack_get(Stack *stack, unsigned int position)
{
	if (position >= stack->size)
		return NULL;

	pthread_rwlock_rdlock(&stack->rwlock);
	StackNode *node = stack->top;
	unsigned int counter = 1;

	while (node != NULL)
	{
		if (counter > position)
			break;
		node = node->next;
		counter++;
	}

	pthread_rwlock_unlock(&stack->rwlock);
	return node != NULL ? node->data : NULL;
}

void stack_print(Stack *stack, DataToString tostring)
{
	pthread_rwlock_rdlock(&stack->rwlock);
	printf("Stack: [\n");
	StackNode *top = stack->top;
	while (top != NULL)
	{
		char *data_str = tostring(top->data);
		printf("  %s", data_str);
		free(data_str);

		StackNode *next = top->next;
		top = next;
		if (top != NULL)
			printf(",\n");
	}
	printf("\n]\n");
	pthread_rwlock_unlock(&stack->rwlock);
}

void stack_destroy(Stack *stack)
{
	pthread_rwlock_trywrlock(&stack->rwlock);

	StackNode *top = stack->top;
	while (top != NULL)
	{
		StackNode *next = top->next;
		if (stack->autofree == true)
			free(top->data);
		free(top);
		top = next;
	}
	
	pthread_rwlock_unlock(&stack->rwlock);
	pthread_rwlock_destroy(&stack->rwlock);
	free(stack);
}
