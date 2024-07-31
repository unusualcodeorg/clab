#ifndef CLAB_DS_LIB_DESTROY_H
#define CLAB_DS_LIB_DESTROY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FreeDataFunc)(void *);

void free_data_func(void *data);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_DESTROY_H
