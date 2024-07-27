#ifndef DS_HASHMAP_H
#define DS_HASHMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct HashNode
	{
		void *key;
		void *value;
		struct HashNode *next; // For collision handling using chaining
	} HashNode;

	typedef struct
	{
		HashNode **buckets;
		unsigned int size;
	} HashMap;

	HashMap *hashmap_create(unsigned int size);
	void hashmap_insert(HashMap *map, void *key, void *value);
	void *hashmap_find(HashMap *map, void *key);
	void hashmap_delete(HashMap *map, void *key);
	void hashmap_destroy(HashMap *map);

#ifdef __cplusplus
}
#endif

#endif // DS_HASHMAP_H
