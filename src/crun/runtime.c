#include "runtime.h"
#include "../dslib/queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void *runner(void *arg)
{
	Runtime *runtime = (Runtime *)arg;
	while (true)
	{
		pthread_mutex_lock(&runtime->mutex);
		while (runtime->pause)
		{
			if (runtime->debug)
				printf("Runtime - %s: has paused.\n", runtime->name);

			pthread_cond_wait(&runtime->cond, &runtime->mutex);

			if (runtime->debug)
				printf("Runtime - %s: has resumed.\n", runtime->name);
		}
		pthread_mutex_unlock(&runtime->mutex);

		if (runtime->execs->size == 0)
		{
			if (runtime->exit == true)
			{
				if (runtime->debug)
					printf("Runtime - %s: will exit.\n", runtime->name);
				break;
			}
			crun_pause(runtime);
			continue;
		}

		Execution *exec = (Execution *)queue_dequeue(runtime->execs);
		if (exec != NULL)
		{
			Croutine croutine = (void (*)(void *))exec->croutine;
			croutine(exec->context);
			free(exec);
		}

		if (runtime->debug)
			printf("Runtime - %s: croutine executed.\n", runtime->name);
	}
	if (runtime->debug)
		printf("Runtime - %s: has exited.\n", runtime->name);
	return NULL;
}

Runtime *crun_create(char *name, bool debug)
{
	Runtime *runtime = malloc(sizeof(Runtime));
	runtime->name = name;
	runtime->debug = debug;
	runtime->execs = queue_create(false);
	runtime->pause = true;
	runtime->exit = false;
	pthread_mutex_init(&runtime->mutex, NULL);
	pthread_cond_init(&runtime->cond, NULL);
	pthread_create(&runtime->thread, NULL, runner, runtime);
	if (runtime->debug)
		printf("Runtime - %s: is created.\n", runtime->name);
	return runtime;
}

void crun_pause(Runtime *runtime)
{
	pthread_mutex_lock(&runtime->mutex);
	runtime->pause = true;
	pthread_mutex_unlock(&runtime->mutex);
}

void crun_resume(Runtime *runtime)
{
	pthread_mutex_lock(&runtime->mutex);
	runtime->pause = false;
	pthread_cond_signal(&runtime->cond);
	pthread_mutex_unlock(&runtime->mutex);
}

void crun_exit(Runtime *runtime)
{
	pthread_mutex_lock(&runtime->mutex);
	runtime->pause = false;
	runtime->exit = true;
	pthread_cond_signal(&runtime->cond);
	pthread_mutex_unlock(&runtime->mutex);
}

void crun_exec(Runtime *runtime, Croutine croutine, void *context)
{
	Execution *exec = (Execution *)malloc(sizeof(Execution));
	exec->croutine = croutine;
	exec->context = context;

	queue_enqueue(runtime->execs, exec);
	if (runtime->pause)
		crun_resume(runtime);
}

int crun_destroy(Runtime *runtime)
{
	crun_exit(runtime);
	pthread_join(runtime->thread, NULL);
	queue_destroy(runtime->execs);
	pthread_mutex_destroy(&runtime->mutex);
	pthread_cond_destroy(&runtime->cond);
	pthread_cancel(runtime->thread);
	if (runtime->debug)
		printf("Runtime - %s: has destroyed.\n", runtime->name);
	free(runtime);
	return EXIT_SUCCESS;
}
