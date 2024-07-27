#include <stdbool.h>
#include <stdarg.h>
#include <pthread.h>

#ifndef DS_LIB_TREE_H
#define DS_LIB_TREE_H

#define TREE_NODE_NULL_ID -1;
#define TREE_ERROR -1;

#ifdef __cplusplus
extern "C"
{
#endif
	typedef char *(*DataToString)(void *);

	typedef struct TreeNode
	{
		unsigned int id;
		void *data;
		unsigned short csize;
		struct TreeNode **children;
		struct TreeNode *parent;
	} TreeNode;

	typedef struct
	{
		bool debug;
		unsigned int counter;
		pthread_rwlock_t rwlock;
		void *lambda;
	} TreeCallbackArg;

	typedef void (*TreeCallback)(TreeNode *, TreeCallbackArg *arg);

	typedef struct
	{
		bool debug;
		bool autofree; // free data on pop
		unsigned int size;
		TreeNode *root;
		pthread_rwlock_t rwlock;
	} Tree;

	Tree *tree_create(bool autofree);
	TreeNode *tree_find(Tree *tree, unsigned int nodeid);
	void *tree_get(Tree *tree, unsigned int nodeid);
	int tree_add_root(Tree *tree, void *data);
	int tree_add(Tree *tree, void *data, unsigned int parentid);
	int tree_remove(Tree *tree, unsigned int nodeid);
	void tree_print(Tree *tree, DataToString tostring);
	void tree_destroy(Tree *tree);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_TREE_H
