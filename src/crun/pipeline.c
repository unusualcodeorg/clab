#include "pipeline.h"

#include <stdlib.h>

Pipeline* pipeline_create(unsigned short prodworkers, unsigned short consworkers,
                          unsigned int capacity) {
  Pipeline* pipe = (Pipeline*)malloc(sizeof(Pipeline));
  pipe->debug = false;
  pipe->bq = bufferq_create(capacity);
  pipe->prodpool = runpool_create(prodworkers);
  pipe->conspool = runpool_create(consworkers);
  return pipe;
}

void pipeline_work_wrapper(void* ctx) {
  PipelineContextWrap* wrapctx = (PipelineContextWrap*)ctx;
  wrapctx->work(wrapctx->bq, wrapctx->context);
  free(wrapctx);
}

void pipeline_add_producer(Pipeline* pipe, PipelineWork work, void* context) {
  PipelineContextWrap* wrapctx = malloc(sizeof(PipelineContextWrap));
  wrapctx->context = context;
  wrapctx->work = work;
  wrapctx->bq = pipe->bq;
  runpool_exec(pipe->prodpool, pipeline_work_wrapper, wrapctx);
}

void pipeline_add_consumer(Pipeline* pipe, PipelineWork work, void* context) {
  PipelineContextWrap* wrapctx = malloc(sizeof(PipelineContextWrap));
  wrapctx->context = context;
  wrapctx->work = work;
  wrapctx->bq = pipe->bq;
  runpool_exec(pipe->conspool, pipeline_work_wrapper, wrapctx);
}

void pipeline_debug(Pipeline* pipe) {
  pipe->debug = true;
  pipe->bq->debug = true;
  runpool_debug(pipe->prodpool, "prod pool");
  runpool_debug(pipe->conspool, "cons pool");
}

void pipeline_join_destory(Pipeline* pipe, FreeDataFunc freedatafunc) {
  runpool_join_destroy(pipe->prodpool);
  runpool_join_destroy(pipe->conspool);
  bufferq_destroy(pipe->bq, freedatafunc);
}
