#include <pthread.h>
#include <stdbool.h>

#include "runtime.h"

#ifndef CLAB_CRUN_RUNPOOL_H
#define CLAB_CRUN_RUNPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *name;
  bool debug;
  size_t size;
  Runtime **runtimes;
} Runpool;

Runpool *runpool_create(size_t size);
void runpool_exec(Runpool *pool, Croutine croutine, void *context);
void runpool_debug(Runpool *pool, char *poolname);
void runpool_join_destroy(Runpool *pool);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_RUNPOOL_H
