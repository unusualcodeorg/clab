#include <pthread.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"

#define LIST_NULL_INDEX -1;

#ifndef CLAB_DS_LIB_LINKEDLIST_H
#define CLAB_DS_LIB_LINKEDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*ListMatcher)(void *item, void *match);

typedef struct ListNode {
  void *data;
  struct ListNode *next;
} ListNode;

typedef struct {
  ListNode *tail;  // keeps the insertion order
  ListNode *head;
  size_t size;
  pthread_rwlock_t rwlock;
} List;

List *list_create(void);
size_t list_add(List *list, void *data);
size_t list_add_at(List *list, void *data, size_t index);
void *list_delete_at(List *list, size_t index, FreeDataFunc freedatafunc);
void *list_get_at(List *list, size_t index);
long list_index_of(List *list, void *match, ListMatcher matcher);
void list_print(List *list, DataToString tostring);
void list_destroy(List *list, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_LINKEDLIST_H
