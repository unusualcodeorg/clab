#ifndef DS_LIB_HASHMAP_H
#define DS_LIB_HASHMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct HashNode
	{
		char *key;
		long value;
		struct HashNode *next; // seperate chining in linked list
	} HashNode;

	typedef struct
	{
		unsigned int size;
		HashNode **buckets; // Array of pointers to linked lists (HashNode)
	} HashMap;

	HashMap *hashmap_create(unsigned int size);
	void hashmap_put(HashMap *map, char *key, long value);
	long hashmap_get(HashMap *map, char *key);
	void hashmap_delete(HashMap *map, char *key);
	void hashmap_print(HashMap *map);
	void hashmap_destroy(HashMap *map);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_HASHMAP_H
