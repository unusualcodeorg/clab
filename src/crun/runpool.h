#include <pthread.h>
#include <stdbool.h>

#include "runtime.h"

#ifndef CRUN_RUNPOOL_H
#define CRUN_RUNPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *name;
  bool debug;
  unsigned short size;
  Runtime **runtimes;

} Runpool;

Runpool *runpool_create(char *name, unsigned short size, bool debug);
void runpool_exec(Runpool *pool, Croutine croutine, void *context);
void runpool_destroy(Runpool *pool);

#ifdef __cplusplus
}
#endif

#endif  // CRUN_RUNPOOL_H
