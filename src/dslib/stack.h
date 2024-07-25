#include <pthread.h>
#include <stdbool.h>

#ifndef DS_LIB_STACK_H
#define DS_LIB_STACK_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char *(*DataToString)(void *);

	typedef struct StackNode
	{
		void *data;
		struct StackNode *next;
	} StackNode;

	typedef struct
	{
		StackNode *top;
		unsigned int size;
		bool autofree; // free data on pop
		pthread_rwlock_t rwlock;
	} Stack;

	Stack *stack_create(bool autofree);
	void stack_push(Stack *stack, void *data);
	void *stack_pop(Stack *stack);
	void *stack_peek(Stack *stack);
	void stack_print(Stack *stack, DataToString tostring);
	void stack_destroy(Stack *stack);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_STACK_H
