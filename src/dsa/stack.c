#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

Stack *stack_create()
{
	Stack *stack = malloc(sizeof(Stack));
	if (stack == NULL)
	{
		perror("failed to allocate memory for stack");
		exit(EXIT_FAILURE);
	}
	stack->top = NULL;
	stack->size = 0;
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

void stack_pop(Stack *stack)
{
	pthread_rwlock_wrlock(&stack->rwlock);
	if (stack->top != NULL)
	{
		StackNode *node = stack->top;
		stack->top = stack->top->next;
		stack->size--;
		free(node->data);
		free(node);
	}
	pthread_rwlock_unlock(&stack->rwlock);
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

void stack_print(Stack *stack, fn_to_string to_string)
{
	pthread_rwlock_rdlock(&stack->rwlock);
	printf("Stack: [\n");
	StackNode *top = stack->top;
	while (top != NULL)
	{
		char *data_str = to_string(top->data);
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
	pthread_rwlock_wrlock(&stack->rwlock);
	StackNode *top = stack->top;
	while (top != NULL)
	{
		StackNode *next = top->next;
		free(top->data);
		free(top);
		top = next;
	}
	pthread_rwlock_unlock(&stack->rwlock);
	pthread_rwlock_destroy(&stack->rwlock);
	free(stack);
}