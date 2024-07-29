#include <pthread.h>
#include <stdbool.h>

#include "datastr.h"

#define LIST_NULL_INDEX -1;

#ifndef CLAB_DS_LIB_LINKEDLIST_H
#define CLAB_DS_LIB_LINKEDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ListNode {
  void *data;
  struct ListNode *next;
} ListNode;

typedef struct {
  ListNode *tail;
  ListNode *head;
  unsigned int size;
  bool autofree;  // free data on pop
  pthread_rwlock_t rwlock;
} List;

List *list_create(bool autofree);
int list_add(List *list, void *data);
int list_add_at(List *list, void *data, unsigned int index);
int list_delete_at(List *list, unsigned int index);
void *list_get_at(List *list, unsigned int index);
void list_print(List *list, DataToString tostring);
void list_destroy(List *list);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_LINKEDLIST_H
