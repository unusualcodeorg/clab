#ifndef CLAB_DEBUG_FUNC_H
#define CLAB_DEBUG_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*ExceFunc)(void);

int debugfn(char *fname);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DEBUG_FUNC_H
