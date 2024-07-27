#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

unsigned int hash_function(void *key, unsigned int size)
{
	return ((uintptr_t)key) % size;
}

HashMap *hashmap_create(unsigned int size)
{
	HashMap *map = (HashMap *)malloc(sizeof(HashMap));
	map->buckets = (HashNode **)calloc(size, sizeof(HashNode *));
	map->size = size;
	return map;
}

void hashmap_insert(HashMap *map, void *key, void *value)
{
	unsigned int index = hash_function(key, map->size);
	HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
	new_node->key = key;
	new_node->value = value;
	new_node->next = map->buckets[index];
	map->buckets[index] = new_node;
}

void *hashmap_find(HashMap *map, void *key)
{
	unsigned int index = hash_function(key, map->size);
	HashNode *node = map->buckets[index];
	while (node)
	{
		if (node->key == key)
		{
			return node->value;
		}
		node = node->next;
	}
	return NULL; 
}

void hashmap_delete(HashMap *map, void *key)
{
	unsigned int index = hash_function(key, map->size);
	HashNode *node = map->buckets[index];
	HashNode *prev = NULL;
	while (node)
	{
		if (node->key == key)
		{
			if (prev)
			{
				prev->next = node->next;
			}
			else
			{
				map->buckets[index] = node->next;
			}
			free(node);
			return;
		}
		prev = node;
		node = node->next;
	}
}

void hashmap_destroy(HashMap *map)
{
	for (unsigned int i = 0; i < map->size; i++)
	{
		HashNode *node = map->buckets[i];
		while (node)
		{
			HashNode *temp = node;
			node = node->next;
			free(temp);
		}
	}
	free(map->buckets);
	free(map);
}
