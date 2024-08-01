#include <time.h>

#ifndef CLAB_CRUN_CLOCK_TIME_H
#define CLAB_CRUN_CLOCK_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Croutine)(void *context);

typedef struct {
  struct timespec start;
  struct timespec end;
} ClockTime;

ClockTime *clocktime_create(void);
void clocktime_start(ClockTime *clocktime);
double clocktime_stop(ClockTime *clocktime);
void clocktime_destroy(ClockTime *clocktime);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_CRUN_CLOCK_TIME_H
