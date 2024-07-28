#include <stdbool.h>

#ifndef DS_LIB_HASHMAP_H
#define DS_LIB_HASHMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char *(*DataToString)(void *);

	typedef struct HashNode
	{
		char *key;
		void *value;
		struct HashNode *next; // seperate chining in linked list
	} HashNode;

	typedef struct
	{
		bool autofree;
		unsigned int size;
		HashNode **buckets; // Array of pointers to linked lists (HashNode)
	} HashMap;

	HashMap *hashmap_create(unsigned int size, bool autofree);
	void hashmap_put(HashMap *map, char *key, void *value);
	void *hashmap_get(HashMap *map, char *key);
	void hashmap_delete(HashMap *map, char *key);
	void hashmap_print(HashMap *map, DataToString tostring);
	void hashmap_destroy(HashMap *map);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_HASHMAP_H
