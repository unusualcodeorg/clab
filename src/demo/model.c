#include "model.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *customer_to_string(void *cust) {
  Customer *customer = (Customer *)cust;

  char *buffer = malloc(100);
  snprintf(buffer, 100, "{ID: %d, Name: %s, Salary: %d, Verified: %s}", customer->id,
           customer->name, customer->salary, customer->verified ? "Yes" : "No");

  return buffer;
}

Customer *new_customer(int id, char *name, int salary, bool verified) {
  Customer *customer = malloc(sizeof(Customer));
  strcpy(customer->name, name);
  customer->id = id;
  customer->salary = salary;
  customer->verified = verified;
  return customer;
}
