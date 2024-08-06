#include <pthread.h>
#include <stdbool.h>

#include "../dslib/bufferq.h"
#include "croutine.h"
#include "runpool.h"

#ifndef CLAB_CRUN_PIPELINE_H
#define CLAB_CRUN_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PipelineWork)(BufferQueue* bq, void* context);

typedef struct {
  void* context;
  PipelineWork work;
  BufferQueue* bq;
} PipelineContextWrap;

typedef struct {
  bool debug;
  BufferQueue* bq;
  Runpool* prodpool;
  Runpool* conspool;
} Pipeline;

Pipeline* pipeline_create(size_t prodworkers, size_t consworkers,
                          size_t capacity);

void pipeline_add_producer(Pipeline* pipe, PipelineWork work, void* context);
void pipeline_add_consumer(Pipeline* pipe, PipelineWork work, void* context);
void pipeline_debug(Pipeline* pipe);
void pipeline_join_destory(Pipeline* pipe,  FreeDataFunc freedatafunc);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_PIPELINE_H
