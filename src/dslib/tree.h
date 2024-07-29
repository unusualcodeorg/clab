#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>

#include "datastr.h"

#ifndef CLAB_DS_LIB_TREE_H
#define CLAB_DS_LIB_TREE_H

#define TREE_NODE_NULL_ID -1;
#define TREE_ERROR -1;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TreeNode {
  unsigned int id;
  void *data;
  unsigned short csize;
  struct TreeNode **children;
  struct TreeNode *parent;
} TreeNode;

typedef struct {
  bool debug;
  unsigned int counter;
  pthread_rwlock_t rwlock;
  void *lambda;
} TreeCallbackArg;

typedef void (*TreeCallback)(TreeNode *, TreeCallbackArg *arg);

typedef struct {
  bool debug;
  bool autofree;  // free data on pop
  unsigned int size;
  TreeNode *root;
  pthread_rwlock_t rwlock;
} Tree;

Tree *tree_create(bool autofree);
TreeNode *tree_find_dfs(Tree *tree, unsigned int nodeid);
TreeNode *tree_find_bfs(Tree *tree, unsigned int nodeid);
void *tree_get(Tree *tree, unsigned int nodeid);
int tree_insert_root(Tree *tree, void *data);
TreeNode *tree_insert_node(Tree *tree, void *data, TreeNode *parent);
int tree_insert(Tree *tree, void *data, unsigned int parentid);
int tree_delete(Tree *tree, unsigned int nodeid);
int tree_max_depth(Tree *tree);
void tree_print_raw(Tree *tree, DataToString tostring);
void tree_print(Tree *tree, DataToString tostring);
void tree_destroy(Tree *tree);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_TREE_H
