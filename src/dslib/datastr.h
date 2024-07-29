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
char *str_location_data_to_string(void *arg);
char *int_location_data_to_string(void *arg);
char *graph_node_char_data_to_string(void *arg);
char *graph_node_num_data_to_string(void *arg);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_DS_LIB_DATA_STRING_H
