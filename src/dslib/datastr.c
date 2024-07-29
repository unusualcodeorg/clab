#include "datastr.h"

#include <stdio.h>
#include <stdlib.h>

#include "path.h"

char *char_data_to_string(void *arg) {
  char data = *(char *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%c", data);
  return buffer;
}

char *int_data_to_string(void *arg) {
  int data = *(int *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%d", data);
  return buffer;
}

char *decimal_data_to_string(void *arg) {
  float data = *(float *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%f", data);
  return buffer;
}

char *str_data_to_string(void *arg) {
  char *data = (char *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%s", data);
  return buffer;
}

char *str_location_data_to_string(void *arg) {
  Location *location = (Location *)arg;
  char *data = (char *)location->data;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]%s", location->id, data);
  return buffer;
}

char *int_location_data_to_string(void *arg) {
  Location *location = (Location *)arg;
  int data = *(int *)location->data;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%d]%d", location->id, data);
  return buffer;
}

char *graph_node_char_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return char_data_to_string(node->data);
}

char *graph_node_num_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return int_data_to_string(node->data);
}
