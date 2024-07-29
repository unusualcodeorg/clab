#include <stdbool.h>
#ifndef CLAB_DEMO_MODEL_H
#define CLAB_DEMO_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char name[50];
  int salary;
  bool verified;
} Customer;

Customer *new_customer(char name[50], int salary, bool verified);
char *customer_to_string(void *cust);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DEMO_MODEL_H