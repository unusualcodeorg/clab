#include "runpool.h"

#include "runtime.h"
#include "stdio.h"
#include "stdlib.h"

Runpool *runpool_create(char *name, unsigned short size, bool debug) {
  if (size < 1) return NULL;

  Runpool *pool = (Runpool *)malloc(sizeof(Runpool));
  pool->name = name;
  pool->debug = debug;
  pool->size = size;
  pool->runtimes = (Runtime **)malloc(size * sizeof(Runtime *));

  for (unsigned short i = 0; i < size; i++) {
    char *rname = malloc(100 * sizeof(char));
    snprintf(rname, 100, "%s:%s%u", name, "Tread", i + 1);
    pool->runtimes[i] = runtime_create(rname, debug);
  }

  return pool;
}

void runpool_exec(Runpool *pool, Croutine croutine, void *context) {
  unsigned short tasksize = 0;
  Runtime *preferred = NULL;
  for (unsigned short i = 0; i < pool->size; i++) {
    Runtime *runtime = pool->runtimes[i];
    if (runtime->pause == true) {
      preferred = runtime;
      break;
    }

    if (preferred == NULL) {
      preferred = runtime;
      tasksize = runtime->execs->size;
      continue;
    }

    if (runtime->execs->size < tasksize) {
      preferred = runtime;
      tasksize = runtime->execs->size;
    }
  }

  runtime_exec(preferred, croutine, context);
}

void runpool_destroy(Runpool *pool) {
  for (unsigned short i = 0; i < pool->size; i++) {
    Runtime *runtime = pool->runtimes[i];
    char *name = runtime->name;
    runtime_destroy(runtime);
    free(name);
  }
  free(pool);
}
