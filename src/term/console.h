#include "../crun/runtime.h"

#ifndef TERM_CONSOLE_H
#define TERM_CONSOLE_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct cprint
	{
		Runtime* runtime;
	} Console;

	int console_create(void);

#ifdef __cplusplus
}
#endif

#endif // TERM_CONSOLE_H
