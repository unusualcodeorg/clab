#include "../dslib/queue.h"
#include <pthread.h>
#include <stdbool.h>

#ifndef CRUN_RUNTIME_H
#define CRUN_RUNTIME_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef void (*Croutine)(void *context);

	typedef struct
	{
		Croutine croutine;
		void *context;
	} Execution;

	typedef struct
	{
		char *name;
		Queue *execs;
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		pthread_t thread;
		bool exit;
		bool pause;
		bool debug;
	} Runtime;

	Runtime *crun_create(char *name, bool debug);
	void crun_pause(Runtime *runtime);
	void crun_resume(Runtime *runtime);
	void crun_exec(Runtime *runtime, Croutine croutine, void *context);
	void crun_block(Runtime *runtime);
	int crun_destroy(Runtime *runtime);

#ifdef __cplusplus
}
#endif

#endif // CRUN_RUNTIME_H
