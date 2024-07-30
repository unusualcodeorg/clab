#include "path.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "queue.h"
#include "stack.h"
#include "tree.h"

char *path_tree_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]", node->id);
  return buffer;
}

Location *location_create(void *data) {
  Location *location = (Location *)malloc(sizeof(Location));
  location->cost = UINT_MAX;
  location->id = 0;
  location->data = data;
  return location;
}

void free_location_data_func(void *data) {
  Location *loc = (Location *)data;
  free(loc->data);
  free(loc);
}
