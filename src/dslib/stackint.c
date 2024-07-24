#include "stackint.h"
#include <stdio.h>
#include <stdlib.h>

StackInt *stackint_create(void)
{
	StackInt *stack = malloc(sizeof(StackInt));
	if (stack == NULL)
	{
		perror("failed to allocate memory for stack");
		exit(EXIT_FAILURE);
	}
	stack->top = NULL;

	return stack;
}

void stackint_push(StackInt *stack, int data)
{
	StackIntNode *node = malloc(sizeof(StackIntNode));

	int *p_data = malloc(sizeof(data));
	*p_data = data;

	node->data = p_data;
	node->next = stack->top;
	stack->top = node;
}

int stackint_pop(StackInt *stack)
{
	int data = 0;
	if (stack->top != NULL)
	{
		StackIntNode *node = stack->top;
		stack->top = stack->top->next;
		data = *node->data;
		free(node->data);
		free(node);
	}
	return data;
}

int stackint_peek(StackInt *stack)
{
	if (stack->top != NULL)
	{
		return *stack->top->data;
	}
	return 0;
}

void stackint_print(StackInt *stack)
{
	printf("Stack: [");

	StackIntNode *top = stack->top;
	while (top != NULL)
	{
		printf("%d", *top->data);
		StackIntNode *next = top->next;
		top = next;
		if (top != NULL)
			printf(",");
	}
	printf("]\n");
}

void stackint_destroy(StackInt *stack)
{
	StackIntNode *top = stack->top;
	while (top != NULL)
	{
		StackIntNode *next = top->next;
		free(top->data);
		free(top);
		top = next;
	}

	free(stack);
}
