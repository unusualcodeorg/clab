#include "tree.h"
#include "stack.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

Tree *tree_create(bool autofree)
{
	Tree *tree = (Tree *)malloc(sizeof(Tree));
	tree->debug = false;
	tree->autofree = autofree;
	tree->size = 0;
	tree->root = NULL;
	pthread_rwlock_init(&tree->rwlock, NULL);
	return tree;
}

TreeCallbackArg *tree_default_callback_arg(Tree *tree)
{
	TreeCallbackArg *arg = (TreeCallbackArg *)malloc(sizeof(TreeCallbackArg));
	arg->counter = 0;
	arg->debug = tree->debug;
	arg->rwlock = tree->rwlock;
	return arg;
}

void tree_traversal_callback(TreeNode *node, TreeCallbackArg *arg)
{
	if (arg->debug == true)
		printf("[%u]->", node->id);
}

TreeNode *tree_node_find_bfs(TreeNode *start, unsigned int nodeid, TreeCallback callback, TreeCallbackArg *arg)
{
	pthread_rwlock_rdlock(&arg->rwlock);
	TreeNode *found = NULL;

	Queue *queue = queue_create(false);
	queue_enqueue(queue, start);

	while (queue->size > 0)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = queue_dequeue(queue);

		if (callback != NULL)
			callback(node, arg);

		if (node->id == nodeid)
		{
			found = node;
			break;
		}

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			if (child->id == nodeid)
			{
				found = child;
				break;
			}
			queue_enqueue(queue, child);
		}

		if (found != NULL)
			break;
	}

	queue_destroy(queue);
	pthread_rwlock_unlock(&arg->rwlock);
	return found;
}

TreeNode *tree_node_find_dfs(TreeNode *start, long nodeid, TreeCallback callback, TreeCallbackArg *arg)
{
	pthread_rwlock_rdlock(&arg->rwlock);
	TreeNode *found = NULL;

	Stack *stack = stack_create(false);
	stack_push(stack, start);

	while (stack->size > 0)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = stack_pop(stack);

		if (callback != NULL)
			callback(node, arg);

		if (node->id == nodeid)
		{
			found = node;
			break;
		}

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			if (child->id == nodeid)
			{
				found = child;
				break;
			}
			stack_push(stack, child);
		}

		if (found != NULL)
			break;
	}

	stack_destroy(stack);
	pthread_rwlock_unlock(&arg->rwlock);
	return found;
}

//  BFS explores all neighbors at the present depth level before moving on to nodes at the next depth level.
TreeNode *tree_find_bfs(Tree *tree, unsigned int nodeid)
{
	if (tree->root == NULL || nodeid >= tree->size)
		return NULL;

	TreeCallbackArg *arg = tree_default_callback_arg(tree);
	TreeNode *node = tree_node_find_bfs(tree->root, nodeid, tree_traversal_callback, arg);

	if (tree->debug == true)
		printf("\nTree: Find BFS Traversal = %u\n", arg->counter);

	free(arg);
	return node;
}

//  DFS explores as far down a branch as possible before backtracking
TreeNode *tree_find_dfs(Tree *tree, unsigned int nodeid)
{
	if (tree->root == NULL || nodeid >= tree->size)
		return NULL;

	TreeCallbackArg *arg = tree_default_callback_arg(tree);
	TreeNode *node = tree_node_find_dfs(tree->root, nodeid, tree_traversal_callback, arg);

	if (tree->debug == true)
		printf("\nTree: Find DFS Traversal = %u\n", arg->counter);

	free(arg);
	return node;
}

int tree_add_root(Tree *tree, void *data)
{
	if (tree->root != NULL)
		return TREE_ERROR;

	pthread_rwlock_trywrlock(&tree->rwlock);
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	node->id = 0;
	node->data = data;
	node->csize = 0;
	node->parent = NULL;
	node->children = NULL;
	tree->root = node;
	tree->size = 1;
	pthread_rwlock_unlock(&tree->rwlock);
	return node->id;
}

void *tree_get(Tree *tree, unsigned int nodeid)
{
	TreeNode *node = tree_find_bfs(tree, nodeid);
	return node != NULL ? node->data : NULL;
}

TreeNode *tree_add_node(Tree *tree, void *data, TreeNode *parent)
{
	if (parent == NULL)
		return NULL;

	pthread_rwlock_wrlock(&tree->rwlock);
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	node->id = tree->size++;
	node->data = data;
	node->csize = 0;
	node->children = NULL;
	node->parent = parent;

	parent->csize++;
	parent->children = (TreeNode **)realloc(parent->children, parent->csize * sizeof(TreeNode *));
	parent->children[parent->csize - 1] = node;

	pthread_rwlock_unlock(&tree->rwlock);
	return node;
}

int tree_add(Tree *tree, void *data, unsigned int parentid)
{
	TreeNode *parent = tree_find_bfs(tree, parentid);
	TreeNode *node = tree_add_node(tree, data, parent);
	return node != NULL ? node->id : TREE_NODE_NULL_ID;
}

int tree_max_depth(Tree *tree)
{
	if (tree == NULL)
		return TREE_ERROR;

	if (tree->size == 0)
		return 0;

	pthread_rwlock_rdlock(&tree->rwlock);

	Stack *stack = stack_create(false);
	stack_push(stack, tree->root);

	int maxdepth = 0;
	int depth = 0;

	while (stack->size > 0)
	{
		if (depth > maxdepth)
			maxdepth = depth;

		TreeNode *node = stack_pop(stack);
		if (node->csize == 0)
		{
			depth = 0;
			continue;
		}

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			stack_push(stack, child);
		}

		depth++;
	}

	stack_destroy(stack);
	pthread_rwlock_unlock(&tree->rwlock);
	return maxdepth;
}

void tree_print_node(TreeNode *node, TreeCallbackArg *arg)
{
	if (node == NULL)
		return;

	if (arg->debug == true)
		arg->counter++;

	DataToString tostring = (DataToString)arg->lambda;

	char *str = tostring(node->data);
	printf(" [%d]%s -->", node->id, str);
	free(str);

	for (unsigned short i = 0; i < node->csize; i++)
	{
		TreeNode *child = node->children[i];
		char *s = tostring(child->data);
		printf(" [%d]%s", child->id, s);
		free(s);
	}
	printf(",\n");
}

void tree_print_raw(Tree *tree, DataToString tostring)
{
	pthread_rwlock_rdlock(&tree->rwlock);
	unsigned int counter = 0;
	printf("\nTree[\n");
	if (tree->root != NULL)
	{
		TreeCallbackArg *arg = tree_default_callback_arg(tree);
		arg->lambda = (void *)tostring;
		tree_node_find_bfs(tree->root, -1, tree_print_node, arg);
		counter = arg->counter;
		free(arg);
	}
	printf("]\n");

	if (tree->debug == true)
		printf("Tree: Raw Print BFS Traversal = %u\n\n", counter);
	pthread_rwlock_unlock(&tree->rwlock);
}

void tree_print(Tree *tree, DataToString tostring)
{
	if (tree == NULL || tree->size == 0)
		return;

	pthread_rwlock_rdlock(&tree->rwlock);

	Stack *stack = stack_create(false);
	stack_push(stack, tree->root);

	unsigned int counter = 0;
	int depth = 0;
	TreeNode *parent = tree->root;

	while (stack->size > 0)
	{
		counter++;
		TreeNode *node = stack_pop(stack);

		// backtracks to the last known parent
		if (node->parent != NULL && parent != node->parent)
		{
			TreeNode *backparent = node->parent;
			while (backparent != NULL && backparent != parent)
			{
				backparent = backparent->parent;
				depth--;
			}
		}

		char *str = tostring(node->data);
		for (int i = 0; i < depth; i++)
			printf("   ");

		TreeNode *next = stack_peek(stack);
		if (next != NULL && next->parent == node->parent)
		{
			printf("%s[%d]%s\n", "├──", node->id, str);
		}
		else
		{
			char *link = depth > 0 ? "└──" : "";
			printf("%s[%d]%s\n", link, node->id, str);
		}

		if (node->csize > 0) // not leaf node
		{
			parent = node; // set self as parent for stack backtrack
			depth++;
		}

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			stack_push(stack, child);
		}
		free(str);
	}

	if (tree->debug == true)
		printf("Tree: Print DFS Traversal = %u\n", counter);

	stack_destroy(stack);
	pthread_rwlock_unlock(&tree->rwlock);
}

void tree_node_destroy(TreeNode *node, bool autofree, TreeCallback callback, TreeCallbackArg *arg)
{
	Stack *history = stack_create(false);
	Stack *stack = stack_create(false);
	stack_push(stack, node);

	while (stack->size > 0)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = stack_pop(stack);
		stack_push(history, node);

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			stack_push(stack, child);
		}
	}

	while (stack->size > 0)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *n = stack_pop(stack);

		if (callback != NULL)
			callback(n, arg);

		if (autofree == true)
			free(n->data);
		free(n->children);
	}

	stack_destroy(history);
	stack_destroy(stack);
}

int tree_remove(Tree *tree, unsigned int nodeid)
{
	TreeNode *node = tree_find_dfs(tree, nodeid);
	if (node == NULL)

		return TREE_NODE_NULL_ID;

	pthread_rwlock_wrlock(&tree->rwlock);
	TreeCallbackArg *arg = tree_default_callback_arg(tree);

	if (node->parent->csize > 1)
	{
		TreeNode **children = malloc((node->parent->csize - 1) * sizeof(TreeNode *));
		unsigned short counter = 0;

		for (unsigned short i = 0; i < node->parent->csize; i++)
		{
			TreeNode *child = node->parent->children[i];
			if (child != node)
				children[counter++] = child;
		}

		free(node->parent->children);
		node->parent->csize--;
		node->parent->children = children;
	}
	else if (node->parent->csize == 1)
	{
		free(node->parent->children);
		node->parent->csize--;
		node->parent->children = NULL;
	}

	tree_node_destroy(node, tree->autofree, tree_traversal_callback, arg);
	pthread_rwlock_unlock(&tree->rwlock);
	return nodeid;
}

void tree_destroy(Tree *tree)
{
	pthread_rwlock_trywrlock(&tree->rwlock);

	TreeCallbackArg *arg = tree_default_callback_arg(tree);
	tree_node_destroy(tree->root, tree->autofree, tree_traversal_callback, arg);

	if (tree->debug == true)
		printf("\nTree: Destroy DFS Traversal = %u\n", arg->counter);

	free(arg);
	pthread_rwlock_unlock(&tree->rwlock);
	pthread_rwlock_destroy(&tree->rwlock);
	free(tree);
}
