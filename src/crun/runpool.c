#include "runpool.h"

#include "runtime.h"
#include "stdio.h"
#include "stdlib.h"

Runpool *runpool_create(size_t size) {
  if (size < 1) return NULL;

  Runpool *pool = (Runpool *)malloc(sizeof(Runpool));
  pool->name = "Runpool";
  pool->debug = false;
  pool->size = size;
  pool->runtimes = (Runtime **)malloc(size * sizeof(Runtime *));

  for (size_t i = 0; i < size; i++) {
    char *rname = malloc(100 * sizeof(char));
    snprintf(rname, 100, "%s:%s-%zu", pool->name, "Thread", i + 1);
    pool->runtimes[i] = runtime_create();
    pool->runtimes[i]->name = rname;
  }

  return pool;
}

void runpool_debug(Runpool *pool, char *poolname) {
  pool->name = poolname;
  pool->debug = true;

  for (size_t i = 0; i < pool->size; i++) {
    free(pool->runtimes[i]->name);
    char *rname = malloc(100 * sizeof(char));
    snprintf(rname, 100, "%s:%s-%zu", pool->name, "Thread", i + 1);
    runtime_debug(pool->runtimes[i], rname);
  }
}

void runpool_exec(Runpool *pool, Croutine croutine, void *context) {
  size_t tasksize = 0;
  Runtime *preferred = NULL;
  for (size_t i = 0; i < pool->size; i++) {
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

    // puts a new task in the queue with least items
    if (runtime->execs->size < tasksize) {
      preferred = runtime;
      tasksize = runtime->execs->size;
    }
  }

  runtime_exec(preferred, croutine, context);
}

void runpool_join_destroy(Runpool *pool) {
  for (size_t i = 0; i < pool->size; i++) {
    Runtime *runtime = pool->runtimes[i];
    char *name = runtime->name;
    runtime_join_destroy(runtime);
    free(name);
  }
  if (pool->debug == true) printf("Runpool - %s: has destroyed.\n", pool->name);
  free(pool);
}
