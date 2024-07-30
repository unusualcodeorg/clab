#ifndef CLAB_DS_LIB_DATA_STRING_H
#define CLAB_DS_LIB_DATA_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char *(*DataToString)(void *);

char *char_data_to_string(void *arg);
char *int_data_to_string(void *arg);
char *decimal_data_to_string(void *arg);
char *str_data_to_string(void *arg);
char *location_str_data_to_string(void *arg);
char *location_int_data_to_string(void *arg);
char *graph_char_data_to_string(void *arg);
char *graph_int_data_to_string(void *arg);
char *graph_str_data_to_string(void *arg);
char *graph_location_str_data_to_string(void *arg);
char *graph_location_char_data_to_string(void *arg);
char *graph_location_int_data_to_string(void *arg);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_DATA_STRING_H
