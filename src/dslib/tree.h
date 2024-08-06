#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>

#include "datastr.h"
#include "destroy.h"

#ifndef CLAB_DS_LIB_TREE_H
#define CLAB_DS_LIB_TREE_H

#define TREE_NODE_NULL_ID 0;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TreeNode {
  size_t id;
  void *data;
  size_t csize;
  struct TreeNode **children;
  struct TreeNode *parent;
} TreeNode;

typedef struct {
  bool debug;
  size_t counter;
  pthread_rwlock_t *rwlock;
  DataToString tostring;
} TreeCallbackArg;

typedef void (*TreeCallback)(TreeNode *, TreeCallbackArg *arg);

typedef struct {
  bool debug;
  size_t size;
  TreeNode *root;
  pthread_rwlock_t rwlock;
} Tree;

Tree *tree_create(void);
TreeNode *tree_find_dfs(Tree *tree, size_t nodeid);
TreeNode *tree_find_bfs(Tree *tree, size_t nodeid);
void *tree_get(Tree *tree, size_t nodeid);
size_t tree_insert_root(Tree *tree, void *data);
TreeNode *tree_insert_node(Tree *tree, void *data, TreeNode *parent);
size_t tree_insert(Tree *tree, void *data, size_t parentid);
size_t tree_delete(Tree *tree, size_t nodeid, FreeDataFunc freedatafunc);
size_t tree_max_depth(Tree *tree);
void tree_print_raw(Tree *tree, DataToString tostring);
void tree_print(Tree *tree, DataToString tostring);
void tree_destroy(Tree *tree, FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_TREE_H
