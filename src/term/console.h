#include "../crun/runtime.h"

#ifndef CLAB_TERM_CONSOLE_H
#define CLAB_TERM_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int ftime;
  char *text;
} Frame;

typedef struct {
  unsigned int ftime;
  Runtime *runtime;
} Console;

Console *console_create(unsigned int ftime_mills);
void console_render(Console *console, char *text);
void console_destroy(Console *console);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_TERM_CONSOLE_H
