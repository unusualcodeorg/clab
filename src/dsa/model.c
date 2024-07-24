#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "model.h"

Customer *new_customer(char name[50], int salary, bool verified);
char *customer_to_string(void *cust);

char *customer_to_string(void *cust)
{
	Customer *customer = (Customer *)cust;

	char *buffer = malloc(100);
	snprintf(buffer, 100, "{Name: %s, Salary: %d, Verified: %s}",
					 customer->name, customer->salary,
					 customer->verified ? "Yes" : "No");

	return buffer;
}

Customer *new_customer(char name[50], int salary, bool verified)
{
	Customer *customer = malloc(sizeof(Customer));
	strcpy(customer->name, name);
	customer->salary = salary;
	customer->verified = verified;
	return customer;
}
