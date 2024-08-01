#include <pthread.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"

#ifndef CLAB_DS_LIB_HASHMAP_H
#define CLAB_DS_LIB_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HashNode {
  char *key;
  void *value;
  struct HashNode *next;  // seperate chining in linked list
} HashNode;

typedef struct {
  unsigned int size;
  HashNode **buckets;  // Array of pointers to linked lists (HashNode)
  pthread_rwlock_t rwlock;
} HashMap;

HashMap *hashmap_create(unsigned int size);
void hashmap_put(HashMap *map, char *key, void *value);
void *hashmap_get(HashMap *map, char *key);
void hashmap_delete(HashMap *map, char *key, FreeDataFunc freedatafunc);
void hashmap_print(HashMap *map, DataToString tostring);
void hashmap_destroy(HashMap *map, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_HASHMAP_H
