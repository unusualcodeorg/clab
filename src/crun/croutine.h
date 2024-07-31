#include <pthread.h>
#include <stdbool.h>

#ifndef CLAB_CRUN_CROUTINE_H
#define CLAB_CRUN_CROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Croutine)(void *context);

typedef struct {
  Croutine croutine;
  void *context;
} Execution;

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_CROUTINE_H
