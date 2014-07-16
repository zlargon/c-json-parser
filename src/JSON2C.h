#ifndef __JSON2C_H
#define __JSON2C_H

// JSON Type
enum {
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NULL
};

// JSON Key Value Pair
typedef struct json_key_value_pair_t {
    char * key;
    char * value;

    int   key_type;
    int value_type;

    struct json_key_value_pair_t * next;

} JSON_Key_Value_Pair;

/*
 * 1. json_type_toString
 *
 * Call to obtain a const string of JSON value type.
 *
 * Parameters:
 *  type - a integer.
 *
 * Returns:
 *  A constant string describing the JSON value type.
 */
const char * json_type_toString(int type);

/*
 * 2. json_number_toDouble
 *
 * Convert JSON number to double.
 *
 * Parameters:
 *  input_string             - the character pointer.
 *  input_string_startIndex  - the start index of the string.
 *  output_double            - the double pointer.
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_number_toDouble(const char * input_string, const int input_string_startIndex, double * output_double);

/*
 * 3. json_getValueByJS
 *
 * Get value by Javascript Syntax with value start & end index and JSON type.
 *
 * Parameters:
 *  input_string             - the character pointer.
 *  input_string_startIndex  - the start index of the string.
 *  input_keys               - the key of the value.
 *  input_keys_startIndex    - the start index of key.
 *  output_value_startIndex  - the integer pointer.
 *  output_value_endIndex    - the integer pointer.
 *  output_value_jsonType    - the integer pointer.
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getValueByJS(const char * input_string, const int input_string_startIndex, const char * input_keys, const int input_keys_startIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);

/*
 * 4. json_object_getValueByKey
 *
 * Get value by key with value start & end index and JSON type.
 *
 * Parameters:
 *  input_string             - the character pointer.
 *  input_string_startIndex  - the start index of the string.
 *  input_key                - the key of the value.
 *  input_key_startIndex     - the start index of key.
 *  input_key_endIndex       - the end index of key.
 *  output_value_startIndex  - the integer pointer.
 *  output_value_endIndex    - the integer pointer.
 *  output_value_jsonType    - the integer pointer.
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_object_getValueByKey(const char * input_string, const int input_string_startIndex, const char * input_key, const int input_key_startIndex, const int input_key_endIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);

/*
 * 5. json_array_getValueByPosition
 *
 * Get array value by position with value start & end index and JSON type.
 *
 * Parameters:
 *  input_string             - the character pointer.
 *  input_string_startIndex  - the start index of the string.
 *  input_array_position     - the value position in array.
 *  output_value_startIndex  - the integer pointer.
 *  output_value_endIndex    - the integer pointer.
 *  output_value_jsonType    - the integer pointer.
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_array_getValueByPosition(const char * input_string, const int input_string_startIndex, const int input_array_position, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);

/*
 * 6. json_getKeyValuePairList
 *
 * object or array get key value pair list
 *
 * Parameters:
 *  input_string                  - the character pointer.
 *  input_string_startIndex       - the start index of the string.
 *  output_keyValuePairList       - the value position in array.
 *  output_keyValuePairList_size  - the integer pointer.
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size);

/*
 * 7. json_keyValuePair_free
 *
 * Free JSON Key Value Pair in recursive.
 *
 * Parameters:
 *  keyValuePair - JSON_Key_Value_Pair pointer.
 *
 * Returns:
 *  always return 0
 */
int json_keyValuePair_free(JSON_Key_Value_Pair * keyValuePair);

#endif
