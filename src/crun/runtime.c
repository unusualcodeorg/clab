#include "runtime.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../dslib/queue.h"

void *runner(void *arg) {
  Runtime *runtime = (Runtime *)arg;
  while (true) {
    pthread_mutex_lock(&runtime->mutex);
    while (runtime->pause) {
      if (runtime->debug == true) printf("Runtime - %s: has paused.\n", runtime->name);

      pthread_cond_wait(&runtime->cond, &runtime->mutex);

      if (runtime->debug == true) printf("Runtime - %s: has resumed.\n", runtime->name);
    }
    pthread_mutex_unlock(&runtime->mutex);

    if (runtime->execs->size == 0) {
      if (runtime->exit == true) {
        if (runtime->debug == true) printf("Runtime - %s: will exit.\n", runtime->name);
        break;
      }
      runtime_pause(runtime);
      continue;
    }

    Execution *exec = (Execution *)queue_dequeue(runtime->execs, NULL);
    if (exec != NULL) {
      Croutine croutine = (void (*)(void *))exec->croutine;
      croutine(exec->context);
      free(exec);
    }

    if (runtime->debug == true) printf("Runtime - %s: croutine executed.\n", runtime->name);
  }
  if (runtime->debug == true) printf("Runtime - %s: has exited.\n", runtime->name);
  return NULL;
}

Runtime *runtime_create(void) {
  Runtime *runtime = malloc(sizeof(Runtime));
  runtime->name = "Runtime";
  runtime->debug = false;
  runtime->execs = queue_create();
  runtime->pause = true;
  runtime->exit = false;

  pthread_mutexattr_init(&runtime->mutexattr);
  pthread_mutexattr_settype(&runtime->mutexattr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&runtime->mutex, &runtime->mutexattr);
  pthread_cond_init(&runtime->cond, NULL);
  pthread_create(&runtime->thread, NULL, runner, runtime);

  if (runtime->debug == true) printf("Runtime - %s: is created.\n", runtime->name);
  return runtime;
}

void runtime_debug(Runtime *runtime, char *name) {
  runtime->name = name;
  runtime->debug = true;
}

void runtime_pause(Runtime *runtime) {
  pthread_mutex_lock(&runtime->mutex);
  runtime->pause = true;
  pthread_mutex_unlock(&runtime->mutex);
}

void runtime_resume(Runtime *runtime) {
  pthread_mutex_lock(&runtime->mutex);
  runtime->pause = false;
  pthread_cond_signal(&runtime->cond);
  pthread_mutex_unlock(&runtime->mutex);
}

void runtime_exit(Runtime *runtime) {
  pthread_mutex_lock(&runtime->mutex);
  runtime->pause = false;
  runtime->exit = true;
  pthread_cond_signal(&runtime->cond);
  pthread_mutex_unlock(&runtime->mutex);
}

void runtime_exec(Runtime *runtime, Croutine croutine, void *context) {
  Execution *exec = (Execution *)malloc(sizeof(Execution));
  exec->croutine = croutine;
  exec->context = context;

  queue_enqueue(runtime->execs, exec);
  if (runtime->pause) runtime_resume(runtime);
}

void runtime_join_destroy(Runtime *runtime) {
  runtime_exit(runtime);
  pthread_join(runtime->thread, NULL);
  queue_destroy(runtime->execs, NULL);

  pthread_mutex_destroy(&runtime->mutex);
  pthread_mutexattr_destroy(&runtime->mutexattr);
  pthread_cond_destroy(&runtime->cond);
  pthread_cancel(runtime->thread);

  if (runtime->debug == true) printf("Runtime - %s: has destroyed.\n", runtime->name);
  free(runtime);
}
