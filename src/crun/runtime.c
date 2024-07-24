#include "runtime.h"
#include "../dsa/queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void *execution(void *arg)
{
	Runtime *runtime = (Runtime *)arg;
	while (true)
	{
		pthread_mutex_lock(&runtime->mutex);
		while (runtime->paused)
		{
			if (runtime->debug)
				printf("Runtime - %s: has paused.\n", runtime->name);
			pthread_cond_wait(&runtime->cond, &runtime->mutex);
		}
		pthread_mutex_unlock(&runtime->mutex);
		if (runtime->execs->size == 0)
		{
			crun_pause(runtime);
			continue;
		}

		if (runtime->debug)
			printf("Runtime - %s: has resumed.\n", runtime->name);

		Croutine croutine = (void (*)(void))queue_dequeue(runtime->execs);
		if (croutine != NULL)
			croutine();
		if (runtime->debug)
			printf("Runtime - %s: executed.\n", runtime->name);
	}
	printf("Runtime - %s: has stopped.\n", runtime->name);
	return NULL;
}

void crun_pause(Runtime *runtime)
{
	pthread_mutex_lock(&runtime->mutex);
	runtime->paused = true;
	pthread_mutex_unlock(&runtime->mutex);
}

void crun_resume(Runtime *runtime)
{
	pthread_mutex_lock(&runtime->mutex);
	runtime->paused = false;
	pthread_cond_signal(&runtime->cond);
	pthread_mutex_unlock(&runtime->mutex);
}

Runtime *crun_create(char *name, bool debug)
{
	Runtime *runtime = malloc(sizeof(Runtime));
	runtime->name = name;
	runtime->debug = debug;
	runtime->execs = queue_create(false);
	runtime->paused = true;
	pthread_mutex_init(&runtime->mutex, NULL);
	pthread_cond_init(&runtime->cond, NULL);
	pthread_create(&runtime->thread, NULL, execution, runtime);
	if (runtime->debug)
		printf("Runtime - %s: is created.\n", runtime->name);
	return runtime;
}

void crun_exec(Runtime *runtime, Croutine croutine)
{
	queue_enqueue(runtime->execs, (void *)croutine);
	if (runtime->paused)
		crun_resume(runtime);
}

int crun_destroy(Runtime *runtime)
{
	queue_destroy(runtime->execs);
	pthread_mutex_destroy(&runtime->mutex);
	pthread_cond_destroy(&runtime->cond);
	pthread_cancel(runtime->thread);
	free(runtime);
	if (runtime->debug)
		printf("Runtime - %s: is destroyed.\n", runtime->name);
	return EXIT_SUCCESS;
}
