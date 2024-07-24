#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#ifndef DS_LIB_STACK_H
#define DS_LIB_STACK_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char *(*fn_to_string)(void *);

	typedef struct StackNode
	{
		void *data;
		struct StackNode *next;
	} StackNode;

	typedef struct
	{
		StackNode *top;
		uint32_t size;
		bool autofree;
		pthread_rwlock_t rwlock;
	} Stack;

	Stack *stack_create(bool autofree);
	void stack_push(Stack *stack, void *data);
	void *stack_pop(Stack *stack);
	void *stack_peek(Stack *stack);
	void stack_print(Stack *stack, fn_to_string to_string);
	void stack_destroy(Stack *stack);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_STACK_H
