#include "tree.h"
#include "stack.h"
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
	return tree;
}

TreeCallbackArg *tree_default_callback_arg(bool debug)
{
	TreeCallbackArg *arg = (TreeCallbackArg *)malloc(sizeof(TreeCallbackArg));
	arg->counter = 0;
	arg->debug = debug;
	return arg;
}

void tree_traversal_callback(TreeNode *node, TreeCallbackArg *arg)
{
	if (arg->debug == true)
		printf("[%u]->", node->id);
}

TreeNode *tree_node_find(TreeNode *start, unsigned int nodeid, TreeCallback callback, TreeCallbackArg *arg)
{
	TreeNode *found = NULL;

	Stack *stack = stack_create(false);
	stack_push(stack, start);

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = stack_pop(stack);
		if (node == NULL)
			continue;

		if (arg->debug == true && callback != NULL)
			callback(node, arg);

		if (node->id == nodeid)
		{
			found = node;
			break;
		}

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			if (child == NULL)
				continue;

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
	return found;
}

TreeNode *tree_find(Tree *tree, unsigned int nodeid)
{
	if (tree->root == NULL || nodeid >= tree->size)
		return NULL;

	if (tree->debug == true)
		printf("\n");

	TreeCallbackArg *arg = tree_default_callback_arg(tree->debug);
	TreeNode *node = tree_node_find(tree->root, nodeid, tree_traversal_callback, arg);

	if (tree->debug == true)
		printf("\nTree: Find Traversal = %u\n", arg->counter);

	free(arg);
	return node;
}

int tree_add_root(Tree *tree, void *data)
{
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	node->id = 0;
	node->data = data;
	node->csize = 0;
	node->children = NULL;
	tree->root = node;
	tree->size = 1;
	return node->id;
}

void *tree_get(Tree *tree, unsigned int nodeid)
{
	TreeNode *node = tree_find(tree, nodeid);
	if (node == NULL)
		return NULL;
	return node->data;
}

int tree_add(Tree *tree, void *data, unsigned int parentid)
{
	if (tree->root == NULL)
	{
		unsigned int id = tree_add_root(tree, data);
		return id;
	}

	TreeNode *parent = tree_find(tree, parentid);
	if (parent == NULL)
		return TREE_ERROR;

	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	node->id = tree->size++;
	node->data = data;
	node->csize = 0;
	node->children = NULL;
	node->parent = parent;

	parent->csize++;
	parent->children = (TreeNode **)realloc(parent->children, parent->csize * sizeof(TreeNode *));
	parent->children[parent->csize - 1] = node;

	return node->id;
}

void tree_traverse(TreeNode *start, TreeCallback callback, TreeCallbackArg *arg)
{
	Stack *stack = stack_create(false);
	stack_push(stack, start);

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = stack_pop(stack);
		if (node == NULL)
			continue;

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			if (child == NULL)
				continue;
			stack_push(stack, child);
		}

		if (callback != NULL)
			callback(node, arg);
	}

	stack_destroy(stack);
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
		if (child == NULL)
			continue;

		char *s = tostring(child->data);
		printf(" [%d]%s", child->id, s);
		free(s);
	}
	printf(",\n");
}

void tree_print(Tree *tree, DataToString tostring)
{
	printf("\nTree[\n");
	if (tree->root != NULL)
	{
		TreeCallbackArg *arg = tree_default_callback_arg(tree->debug);
		arg->lambda = (void *)tostring;
		tree_traverse(tree->root, tree_print_node, arg);

		if (tree->debug == true)
			printf("\nTree: Print Traversal = %u\n", arg->counter);

		free(arg);
	}
	printf("]\n");
}

void tree_node_destroy(TreeNode *node, bool autofree, TreeCallback callback, TreeCallbackArg *arg)
{
	Stack *history = stack_create(false);
	Stack *stack = stack_create(false);
	stack_push(stack, node);

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *node = stack_pop(stack);
		if (node == NULL)
			continue;

		stack_push(history, node);

		for (unsigned short i = 0; i < node->csize; i++)
		{
			TreeNode *child = node->children[i];
			if (child == NULL)
				continue;
			stack_push(stack, child);
		}
	}

	while (stack->top != NULL)
	{
		if (arg->debug == true)
			arg->counter++;

		TreeNode *n = stack_pop(stack);
		if (n == NULL)
			continue;

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
	TreeNode *node = tree_find(tree, nodeid);
	if (node == NULL)
		return TREE_NODE_NULL_ID;

	TreeCallbackArg *arg = tree_default_callback_arg(tree->debug);

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
	return nodeid;
}

void tree_destroy(Tree *tree)
{
	if (tree->debug == true)
		printf("\n");

	TreeCallbackArg *arg = tree_default_callback_arg(tree->debug);
	tree_node_destroy(tree->root, tree->autofree, tree_traversal_callback, arg);

	if (tree->debug == true)
		printf("\nTree: Destroy Traversal = %u\n", arg->counter);

	free(arg);
	free(tree);
}
