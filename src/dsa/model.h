#include <stdbool.h>
#ifndef MODEL_H
#define MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		char name[50];
		int salary;
		bool verified;
	} Customer;

	Customer *new_customer(char name[50], int salary, bool verified);
	char *customer_to_string(void *cust);

#ifdef __cplusplus
}
#endif

#endif // MODEL_H