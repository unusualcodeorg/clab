#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

Stack *stack_new()
{
	Stack *stack = malloc(sizeof(Stack));
	if (stack == NULL)
	{
		perror("failed to allocate memory for stack");
		exit(EXIT_FAILURE);
	}
	stack->top = NULL;
	stack->size = 0;

	return stack;
}

void stack_push(Stack *stack, void *data)
{
	StackNode *node = malloc(sizeof(StackNode));
	node->data = data;
	node->next = stack->top;
	stack->top = node;
	stack->size++;
}

void stack_pop(Stack *stack)
{
	if (stack->top == NULL)
		return;

	StackNode *node = stack->top;
	stack->top = stack->top->next;
	stack->size--;
	free(node->data);
	free(node);
}

void *stack_peek(Stack *stack)
{
	if (stack->top == NULL)
		return NULL;

	return stack->top->data;
}

void stack_print(Stack *stack, fn_to_string to_string)
{
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
}

void stack_destroy(Stack *stack)
{
	StackNode *top = stack->top;
	while (top != NULL)
	{
		StackNode *next = top->next;
		free(top->data);
		free(top);
		top = next;
	}

	free(stack);
}