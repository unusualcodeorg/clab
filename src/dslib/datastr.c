#include "datastr.h"

#include <limits.h>
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

char *location_str_data_to_string(void *arg) {
  Location *location = (Location *)arg;
  char *data = (char *)location->data;
  long cost = location->cost == SIZE_MAX ? -1 : (long)location->cost;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%zu]-%ld-%s", location->id, cost, data);
  return buffer;
}

char *location_char_data_to_string(void *arg) {
  Location *location = (Location *)arg;
  char data = *(char *)arg;
  long cost = location->cost == SIZE_MAX ? -1 : (long)location->cost;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%zu]-%ld-%c", location->id, cost, data);
  return buffer;
}

char *location_int_data_to_string(void *arg) {
  Location *location = (Location *)arg;
  int data = *(int *)location->data;
  long cost = location->cost == SIZE_MAX ? -1 : (long)location->cost;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "[%zu]-%ld-%d", location->id, cost, data);
  return buffer;
}

char *graph_char_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return char_data_to_string(node->data);
}

char *graph_int_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return int_data_to_string(node->data);
}

char *graph_str_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return str_data_to_string(node->data);
}

char *graph_location_str_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return location_str_data_to_string(node->data);
}

char *graph_location_char_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return location_char_data_to_string(node->data);
}

char *graph_location_int_data_to_string(void *arg) {
  GraphNode *node = (GraphNode *)arg;
  return location_int_data_to_string(node->data);
}
