#include "graph.h"

#ifndef DS_LIB_UTIL_H
#define DS_LIB_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

	Graph *graph_from_2d_arr(char **arr, int rows, int cols, bool autofree);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_UTIL_H
