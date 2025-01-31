#include "tree.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "stack.h"

Tree *tree_create(void) {
  Tree *tree = (Tree *)malloc(sizeof(Tree));
  tree->debug = false;
  tree->size = 0;
  tree->root = NULL;
  pthread_rwlock_init(&tree->rwlock, NULL);
  return tree;
}

TreeCallbackArg *tree_default_callback_arg(Tree *tree) {
  TreeCallbackArg *arg = (TreeCallbackArg *)malloc(sizeof(TreeCallbackArg));
  arg->counter = 0;
  arg->debug = tree->debug;
  return arg;
}

void tree_traversal_callback(TreeNode *node, TreeCallbackArg *arg) {
  if (arg->debug == true) printf("[%zu]->", node->id);
}

TreeNode *tree_node_find_bfs(TreeNode *start, size_t nodeid, TreeCallback callback,
                             TreeCallbackArg *arg) {
  TreeNode *found = NULL;

  Queue *queue = queue_create();
  queue_enqueue(queue, start);

  while (queue->size > 0) {
    if (arg->debug == true) arg->counter++;

    TreeNode *node = queue_dequeue(queue, NULL);

    if (callback != NULL) callback(node, arg);

    if (node->id == nodeid) {
      found = node;
      break;
    }

    for (size_t i = 0; i < node->csize; i++) {
      TreeNode *child = node->children[i];
      if (child->id == nodeid) {
        found = child;
        break;
      }
      queue_enqueue(queue, child);
    }

    if (found != NULL) break;
  }

  queue_destroy(queue, NULL);
  return found;
}

TreeNode *tree_node_find_dfs(TreeNode *start, size_t nodeid, TreeCallback callback,
                             TreeCallbackArg *arg) {
  TreeNode *found = NULL;

  Stack *stack = stack_create();
  stack_push(stack, start);

  while (stack->size > 0) {
    if (arg->debug == true) arg->counter++;

    TreeNode *node = stack_pop(stack, NULL);

    if (callback != NULL) callback(node, arg);

    if (node->id == nodeid) {
      found = node;
      break;
    }

    for (size_t i = 0; i < node->csize; i++) {
      TreeNode *child = node->children[i];
      if (child->id == nodeid) {
        found = child;
        break;
      }
      stack_push(stack, child);
    }

    if (found != NULL) break;
  }

  stack_destroy(stack, NULL);
  return found;
}

//  BFS explores all neighbors at the present depth level before moving on to nodes at the next
//  depth level.
TreeNode *tree_find_bfs(Tree *tree, size_t nodeid) {
  if (tree->root == NULL || nodeid > tree->size) return NULL;
  pthread_rwlock_rdlock(&tree->rwlock);

  TreeCallbackArg *arg = tree_default_callback_arg(tree);
  TreeNode *node = tree_node_find_bfs(tree->root, nodeid, tree_traversal_callback, arg);

  if (tree->debug == true) printf("\nTree: Find BFS Traversal = %zu\n", arg->counter);

  free(arg);
  pthread_rwlock_unlock(&tree->rwlock);
  return node;
}

//  DFS explores as far down a branch as possible before backtracking
TreeNode *tree_find_dfs(Tree *tree, size_t nodeid) {
  if (tree->root == NULL || nodeid > tree->size) return NULL;
  pthread_rwlock_rdlock(&tree->rwlock);

  TreeCallbackArg *arg = tree_default_callback_arg(tree);
  TreeNode *node = tree_node_find_dfs(tree->root, nodeid, tree_traversal_callback, arg);

  if (tree->debug == true) printf("\nTree: Find DFS Traversal = %zu\n", arg->counter);

  free(arg);
  pthread_rwlock_unlock(&tree->rwlock);
  return node;
}

size_t tree_insert_root(Tree *tree, void *data) {
  if (tree->root != NULL) return TREE_NODE_NULL_ID;

  pthread_rwlock_wrlock(&tree->rwlock);
  TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
  node->id = 1;
  node->data = data;
  node->csize = 0;
  node->parent = NULL;
  node->children = NULL;
  tree->root = node;
  tree->size = 1;
  pthread_rwlock_unlock(&tree->rwlock);
  return node->id;
}

void *tree_get(Tree *tree, size_t nodeid) {
  TreeNode *node = tree_find_bfs(tree, nodeid);
  return node != NULL ? node->data : NULL;
}

TreeNode *tree_insert_node(Tree *tree, void *data, TreeNode *parent) {
  if (parent == NULL) return NULL;

  pthread_rwlock_wrlock(&tree->rwlock);
  TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
  node->id = ++tree->size;
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

size_t tree_insert(Tree *tree, void *data, size_t parentid) {
  TreeNode *parent = tree_find_bfs(tree, parentid);
  TreeNode *node = tree_insert_node(tree, data, parent);
  return node != NULL ? node->id : TREE_NODE_NULL_ID;
}

size_t tree_max_depth(Tree *tree) {
  if (tree == NULL || tree->size == 0) return 0;

  pthread_rwlock_rdlock(&tree->rwlock);

  Stack *stack = stack_create();
  stack_push(stack, tree->root);

  size_t maxdepth = 0;
  size_t depth = 0;

  while (stack->size > 0) {
    if (depth > maxdepth) maxdepth = depth;

    TreeNode *node = stack_pop(stack, NULL);
    if (node->csize == 0) {
      depth = 0;
      continue;
    }

    for (size_t i = 0; i < node->csize; i++) {
      TreeNode *child = node->children[i];
      stack_push(stack, child);
    }

    depth++;
  }

  stack_destroy(stack, NULL);
  pthread_rwlock_unlock(&tree->rwlock);
  return maxdepth;
}

void tree_print_node(TreeNode *node, TreeCallbackArg *arg) {
  if (node == NULL) return;

  if (arg->debug == true) arg->counter++;

  char *str = arg->tostring(node->data);
  printf(" [%zu]%s -->", node->id, str);
  free(str);

  for (size_t i = 0; i < node->csize; i++) {
    TreeNode *child = node->children[i];
    char *s = arg->tostring(child->data);
    printf(" [%zu]%s", child->id, s);
    free(s);
  }
  printf(",\n");
}

void tree_print_raw(Tree *tree, DataToString tostring) {
  pthread_rwlock_rdlock(&tree->rwlock);
  size_t counter = 0;
  printf("\nTree[\n");
  if (tree->root != NULL) {
    TreeCallbackArg *arg = tree_default_callback_arg(tree);
    arg->tostring = tostring;
    // 0 id means traverse the whole tree
    tree_node_find_bfs(tree->root, 0, tree_print_node, arg);
    counter = arg->counter;
    free(arg);
  }
  printf("]\n");

  if (tree->debug == true) printf("Tree: Raw Print BFS Traversal = %zu\n", counter);
  pthread_rwlock_unlock(&tree->rwlock);
}

void tree_print(Tree *tree, DataToString tostring) {
  if (tree == NULL || tree->size == 0) return;

  pthread_rwlock_rdlock(&tree->rwlock);

  Stack *stack = stack_create();
  stack_push(stack, tree->root);

  size_t counter = 0;
  size_t depth = 0;
  TreeNode *parent = tree->root;

  while (stack->size > 0) {
    counter++;
    TreeNode *node = stack_pop(stack, NULL);

    // backtracks to the last known parent
    if (node->parent != NULL && parent != node->parent) {
      TreeNode *backparent = node->parent;
      while (backparent != NULL && backparent != parent) {
        backparent = backparent->parent;
        depth--;
      }
    }

    char *str = tostring(node->data);
    for (size_t i = 0; i < depth; i++) printf("   ");

    TreeNode *next = stack_peek(stack);
    if (next != NULL && next->parent == node->parent) {
      printf("%s[%zu]%s\n", "├──", node->id, str);
    } else {
      char *link = depth > 0 ? "└──" : "";
      printf("%s[%zu]%s\n", link, node->id, str);
    }

    if (node->csize > 0)  // not leaf node
    {
      parent = node;  // set self as parent for stack backtrack
      depth++;
    }

    for (size_t i = 0; i < node->csize; i++) {
      TreeNode *child = node->children[i];
      stack_push(stack, child);
    }
    free(str);
  }

  if (tree->debug == true) printf("Tree: Print DFS Traversal = %zu\n", counter);

  stack_destroy(stack, NULL);
  pthread_rwlock_unlock(&tree->rwlock);
}

void tree_node_destroy(TreeNode *node, TreeCallback callback, TreeCallbackArg *arg,
                       FreeDataFunc freedatafunc) {
  Stack *history = stack_create();
  Stack *stack = stack_create();
  stack_push(stack, node);

  while (stack->size > 0) {
    if (arg->debug == true) arg->counter++;

    TreeNode *node = stack_pop(stack, NULL);
    stack_push(history, node);

    for (size_t i = 0; i < node->csize; i++) {
      TreeNode *child = node->children[i];
      stack_push(stack, child);
    }
  }

  while (stack->size > 0) {
    if (arg->debug == true) arg->counter++;

    TreeNode *n = stack_pop(stack, NULL);

    if (callback != NULL) callback(n, arg);

    if (freedatafunc != NULL) freedatafunc(n->data);
    free(n->children);
  }

  stack_destroy(history, NULL);
  stack_destroy(stack, NULL);
}

size_t tree_delete(Tree *tree, size_t nodeid, FreeDataFunc freedatafunc) {
  TreeNode *node = tree_find_dfs(tree, nodeid);
  if (node == NULL) return TREE_NODE_NULL_ID;

  pthread_rwlock_wrlock(&tree->rwlock);
  TreeCallbackArg *arg = tree_default_callback_arg(tree);

  if (node->parent->csize > 1) {
    TreeNode **children = malloc((node->parent->csize - 1) * sizeof(TreeNode *));
    size_t counter = 0;

    for (size_t i = 0; i < node->parent->csize; i++) {
      TreeNode *child = node->parent->children[i];
      if (child != node) children[counter++] = child;
    }

    free(node->parent->children);
    node->parent->csize--;
    node->parent->children = children;
  } else if (node->parent->csize == 1) {
    free(node->parent->children);
    node->parent->csize--;
    node->parent->children = NULL;
  }

  tree_node_destroy(node, tree_traversal_callback, arg, freedatafunc);
  pthread_rwlock_unlock(&tree->rwlock);
  return nodeid;
}

void tree_destroy(Tree *tree, FreeDataFunc freedatafunc) {
  pthread_rwlock_trywrlock(&tree->rwlock);

  TreeCallbackArg *arg = tree_default_callback_arg(tree);
  tree_node_destroy(tree->root, tree_traversal_callback, arg, freedatafunc);

  if (tree->debug == true) printf("\nTree: Destroy DFS Traversal = %zu\n", arg->counter);

  free(arg);
  pthread_rwlock_unlock(&tree->rwlock);
  pthread_rwlock_destroy(&tree->rwlock);
  free(tree);
}
