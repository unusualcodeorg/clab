#include "clocktime.h"

#include <stdlib.h>

ClockTime *clocktime_create() {
  ClockTime *clocktime = (ClockTime *)malloc(sizeof(ClockTime));
  return clocktime;
}

void clocktime_start(ClockTime *clocktime) { clock_gettime(CLOCK_MONOTONIC, &clocktime->start); }

double clocktime_stop(ClockTime *clocktime) {
  clock_gettime(CLOCK_MONOTONIC, &clocktime->end);
  double sec = clocktime->end.tv_sec - clocktime->start.tv_sec;
  double nano = (clocktime->end.tv_nsec - clocktime->start.tv_nsec) / 1e9;
  return sec + nano;
}

void clocktime_destroy(ClockTime *clocktime) { free(clocktime); }