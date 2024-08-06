#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DJB2 hash function for strings
size_t hash_function(char *key, size_t size) {
  unsigned long hash = 5381;
  int c;
  while ((c = *key++)) hash = ((hash << 5) + hash) + c;
  return hash % size;
}

HashMap *hashmap_create(size_t size) {
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  map->buckets = (HashNode **)calloc(size, sizeof(HashNode *));
  map->size = size;
  pthread_rwlock_init(&map->rwlock, NULL);
  return map;
}

void hashmap_put(HashMap *map, char *key, void *value) {
  pthread_rwlock_wrlock(&map->rwlock);
  char *dupkey = strdup(key);
  size_t index = hash_function(dupkey, map->size);
  HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
  new_node->key = dupkey;
  new_node->value = value;
  new_node->next = map->buckets[index];
  map->buckets[index] = new_node;
  pthread_rwlock_unlock(&map->rwlock);
}

void *hashmap_get(HashMap *map, char *key) {
  pthread_rwlock_rdlock(&map->rwlock);
  size_t index = hash_function(key, map->size);
  HashNode *node = map->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      pthread_rwlock_unlock(&map->rwlock);
      return node->value;
    }
    node = node->next;
  }
  pthread_rwlock_unlock(&map->rwlock);
  return NULL;
}

void hashmap_delete(HashMap *map, char *key, FreeDataFunc freedatafunc) {
  pthread_rwlock_wrlock(&map->rwlock);
  size_t index = hash_function(key, map->size);
  HashNode *node = map->buckets[index];
  HashNode *prev = NULL;
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      if (prev == NULL)
        map->buckets[index] = node->next;
      else
        prev->next = node->next;

      if (freedatafunc != NULL) freedatafunc(node->value);
      free(node->key);
      free(node);
      pthread_rwlock_unlock(&map->rwlock);
      return;
    }
    prev = node;
    node = node->next;
  }
  pthread_rwlock_unlock(&map->rwlock);
}

void hashmap_print(HashMap *map, DataToString tostring) {
  pthread_rwlock_rdlock(&map->rwlock);
  for (size_t i = 0; i < map->size; i++) {
    HashNode *node = map->buckets[i];
    if (node == NULL) {
      printf("HashMap %zu: Empty\n", i);
    } else {
      printf("HashMap %zu:\n", i);
      while (node != NULL) {
        char *datastr = tostring(node->value);
        printf("    Key: %s, Value: %s\n", node->key, datastr);
        node = node->next;
        free(datastr);
      }
    }
  }
  pthread_rwlock_unlock(&map->rwlock);
}

void hashmap_destroy(HashMap *map, FreeDataFunc freedatafunc) {
  pthread_rwlock_trywrlock(&map->rwlock);
  for (size_t i = 0; i < map->size; i++) {
    HashNode *node = map->buckets[i];
    while (node) {
      HashNode *temp = node;
      node = node->next;
      if (freedatafunc != NULL) freedatafunc(temp->value);
      free(temp->key);
      free(temp);
    }
  }
  free(map->buckets);
  pthread_rwlock_unlock(&map->rwlock);
  pthread_rwlock_destroy(&map->rwlock);
  free(map);
}
