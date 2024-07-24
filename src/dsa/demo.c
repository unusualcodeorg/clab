#include "demo.h"
#include "stack.h"
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
	stack_pop(stack);

	Customer *customer = stack_peek(stack);
	printf("{Name: %s, Salary: %d, Verified: %s}\n",
				 customer->name, customer->salary,
				 customer->verified ? "Yes" : "No");

	stack_print(stack, customer_to_string);

	return 0;
}