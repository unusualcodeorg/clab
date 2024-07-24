#ifndef STACKINT_H
#define STACKINT_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct StackIntNode
	{
		int *data;
		struct StackIntNode *next;
	} StackIntNode;

	typedef struct
	{
		StackIntNode *top;
	} StackInt;

	StackInt *stackint_new();
	void stackint_push(StackInt *stack, int data);
	int stackint_pop(StackInt *stack);
	int stackint_peek(StackInt *stack);
	void stackint_print(StackInt *stack);
	void stackint_destroy(StackInt *stack);

#ifdef __cplusplus
}
#endif

#endif // STACKINT_H