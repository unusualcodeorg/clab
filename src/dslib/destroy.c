#include "destroy.h"

#include <stdlib.h>

void free_data_func(void *data) {
  if (data != NULL) free(data);
}
