#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <cJSON.h>

typedef struct JsonNameValuePairT {
    const char* _name;
    const char* _value;
}JsonNameValuePair;

char *load_json_from_file(const char *const json_file);
void store_json_to_file(char* json, const char* file);
int int_from_json_value(const cJSON *json_value, unsigned int max_range);
int int_from_json_array(const char *json_array, const char *token_name, int default_value, unsigned int max_range);
double double_from_json_value(const cJSON *json_value, double max_range);
int color_from_json_value(const cJSON *json_value, int default_value);
const char *string_from_json_array(const char *json_array, const char *token_name);
JsonNameValuePair *create_name_value_pairs_from_json_array(const char *json_array);
void free_name_value_pairs(JsonNameValuePair *pairs);
int num_name_value_pairs(JsonNameValuePair *pairs);
cJSON* read_json(const char* data_json_file);

#ifdef __cplusplus
} //extern "C"
#endif