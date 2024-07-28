#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DJB2 hash function for strings
unsigned int hash_function(char *key, unsigned int size) {
  unsigned long hash = 5381;
  int c;
  while ((c = *key++)) hash = ((hash << 5) + hash) + c;
  return hash % size;
}

HashMap *hashmap_create(unsigned int size, bool autofree) {
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  map->buckets = (HashNode **)calloc(size, sizeof(HashNode *));
  map->size = size;
  map->autofree = autofree;
  return map;
}

void hashmap_put(HashMap *map, char *key, void *value) {
  char *dupkey = strdup(key);
  unsigned int index = hash_function(dupkey, map->size);
  HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
  new_node->key = dupkey;
  new_node->value = value;
  new_node->next = map->buckets[index];
  map->buckets[index] = new_node;
}

void *hashmap_get(HashMap *map, char *key) {
  unsigned int index = hash_function(key, map->size);
  HashNode *node = map->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) return node->value;
    node = node->next;
  }
  return NULL;
}

void hashmap_delete(HashMap *map, char *key) {
  unsigned int index = hash_function(key, map->size);
  HashNode *node = map->buckets[index];
  HashNode *prev = NULL;
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      if (prev == NULL)
        map->buckets[index] = node->next;
      else
        prev->next = node->next;

      if (map->autofree == true) free(node->value);
      free(node->key);
      free(node);
      return;
    }
    prev = node;
    node = node->next;
  }
}

void hashmap_print(HashMap *map, DataToString tostring) {
  for (unsigned int i = 0; i < map->size; i++) {
    HashNode *node = map->buckets[i];
    if (node == NULL) {
      printf("HashMap %d: Empty\n", i);
    } else {
      printf("HashMap %d:\n", i);
      while (node != NULL) {
        char *datastr = tostring(node->value);
        printf("    Key: %s, Value: %s\n", node->key, datastr);
        node = node->next;
        free(datastr);
      }
    }
  }
}

void hashmap_destroy(HashMap *map) {
  for (unsigned int i = 0; i < map->size; i++) {
    HashNode *node = map->buckets[i];
    while (node) {
      HashNode *temp = node;
      node = node->next;
      free(temp->key);
      free(temp);
    }
  }
  free(map->buckets);
  free(map);
}
