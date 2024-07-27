#include "graph.h"

#ifndef DS_LIB_UTIL_H
#define DS_LIB_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

	void **util_create_2d_pt_arr(int rows, int cols, unsigned long elemsize);
	void util_free_2d_pt_arr(void **arr, int rows);
	Graph *util_graph_from_2d_arr(char **arr, int rows, int cols, bool autofree);

#ifdef __cplusplus
}
#endif

#endif // DS_LIB_UTIL_H
