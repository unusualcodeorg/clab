#include "list.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List *list_create(bool autofree) {
  List *list = (List *)malloc(sizeof(List));
  if (list == NULL) {
    perror("failed to allocate memory for list");
    exit(EXIT_FAILURE);
  }
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  list->autofree = autofree;
  pthread_rwlock_init(&list->rwlock, NULL);
  return list;
}

int list_add(List *list, void *data) {
  pthread_rwlock_wrlock(&list->rwlock);

  ListNode *node = (ListNode *)malloc(sizeof(ListNode));
  node->data = data;
  node->next = NULL;

  if (list->head == NULL) {
    list->head = node;
    list->size++;
    pthread_rwlock_unlock(&list->rwlock);
    return 0;
  }

  if (list->tail == NULL)
    list->head->next = node;
  else
    list->tail->next = node;

  list->tail = node;
  list->size++;

  pthread_rwlock_unlock(&list->rwlock);
  return list->size - 1;
}

int list_add_at(List *list, void *data, unsigned int index) {
  if (index >= list->size) return LIST_NULL_INDEX;

  pthread_rwlock_wrlock(&list->rwlock);
  ListNode *node = (ListNode *)malloc(sizeof(ListNode));
  node->data = data;
  node->next = NULL;

  if (index == 0) {
    node->next = list->head;
    list->head = node;
    if (list->tail == NULL) list->tail = node->next;
  } else {
    list->tail->next = node;
    list->tail = node;
  }

  list->size++;
  pthread_rwlock_unlock(&list->rwlock);
  return index;
}

int list_delete_at(List *list, unsigned int index) {
  if (index >= list->size) return LIST_NULL_INDEX;
  pthread_rwlock_wrlock(&list->rwlock);

  ListNode *found = NULL;
  ListNode *prev = NULL;
  ListNode *current = NULL;

  for (unsigned int i = 0; i < list->size; i++) {
    if (i == 0) {
      current = list->head;
    } else {
      prev = current;
      current = current->next;
    }

    if (i == index) {
      found = current;
      break;
    }
  }

  if (found == NULL) {
    pthread_rwlock_unlock(&list->rwlock);
    return LIST_NULL_INDEX;
  }

  if (found == list->head) {
    list->head = list->head->next;
    if (list->head == list->tail) {
      list->tail = NULL;
    }
  } else if (found == list->tail) {
    list->tail = prev;
    prev->next = NULL;
  } else {
    prev->next = found->next;
  }

  list->size--;

  if (list->autofree == true) free(found->data);
  free(found);

  pthread_rwlock_unlock(&list->rwlock);
  return index;
}

void *list_get_at(List *list, unsigned int index) {
  if (index >= list->size) return NULL;

  pthread_rwlock_rdlock(&list->rwlock);
  ListNode *current = list->head;
  for (unsigned int i = 1; i <= index; i++) {
    current = current->next;
  }
  pthread_rwlock_unlock(&list->rwlock);
  return current->data;
}

void list_print(List *list, DataToString tostring) {
  pthread_rwlock_rdlock(&list->rwlock);
  printf("List: [\n");

  ListNode *current = list->head;
  while (current) {
    char *data_str = tostring(current->data);
    printf("  %s", data_str);
    free(data_str);

    current = current->next;
    if (current != NULL) printf(",\n");
  }

  printf("\n]\n");
  pthread_rwlock_unlock(&list->rwlock);
}

void list_destroy(List *list) {
  pthread_rwlock_trywrlock(&list->rwlock);

  ListNode *current = list->head;
  while (current) {
    ListNode *next = current->next;
    if (list->autofree) free(current->data);
    free(current);
    current = next;
  }

  pthread_rwlock_unlock(&list->rwlock);
  pthread_rwlock_destroy(&list->rwlock);
  free(list);
}