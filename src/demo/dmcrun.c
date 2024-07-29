#include "dmcrun.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../crun/runpool.h"
#include "../crun/runtime.h"

/*--------------------RUNTIME DEMO------------------------ */

void work(void *context) {
  (void)context;  // to supress unused variable warning
  printf("Croutine: started\n");
  sleep(1);
  printf("Croutine: completed\n");
}

int runtime_demo(void) {
  printf("\n--------------RUNTIME DEMO----------------\n");
  Runtime *runtime = runtime_create("Demo", true);

  for (int i = 0; i < 3; i++) {
    printf("work scheduled\n");
    runtime_exec(runtime, work, NULL);
    sleep(2);
  }

  runtime_destroy(runtime);
  printf("--------------RUNTIME DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*--------------------RUNTIME DEMO------------------------ */

/*--------------------RUNPOOL DEMO------------------------ */
void poolwork(void *context) {
  int data = *(int *)context;
  printf("Croutine: started: CTX - %d\n", data);
  int stime = data % 2 + 1;
  sleep(stime);
  printf("Croutine: completed: CTX - %d\n", data);
}

int runpool_demo(void) {
  printf("\n--------------RUNPOOL DEMO----------------\n");
  Runpool *pool = runpool_create("Pool", 5, true);

  for (int i = 0; i < 20; i++) {
    printf("work scheduled\n");
    runpool_exec(pool, poolwork, &i);
    usleep(500000);
  }

  runpool_destroy(pool);
  printf("--------------RUNPOOL DEMO----------------\n");
  return EXIT_SUCCESS;
}
/*--------------------RUNPOOL DEMO------------------------ */
