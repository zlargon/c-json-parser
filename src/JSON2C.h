#ifndef __JSON2C_H
#define __JSON2C_H

// JSON Value Types
typedef enum {
    JSON_VALUE_TYPE_OBJECT,
    JSON_VALUE_TYPE_ARRAY,
    JSON_VALUE_TYPE_NUMBER,
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_BOOLEAN,
    JSON_VALUE_TYPE_NULL
} JsonValueType;

/*
 * Function: json_valueTypeDescription
 *
 * Call to obtain a const string description of JSON value type.
 *
 * Parameters:
 *  type - a integer.
 *
 * Returns:
 *  A constant string describing the JSON value type.
 */
const char * json_valueTypeDescription(JsonValueType type);

/*
 * Function: json_getValue
 *
 * Get the value with end index and JSON type.
 * Find JSON Object and Array in shallow
 * (only get the left and right bracket, but do not check inside)
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  output_endIndex - the integer pointer.
 *   -1 - failure
 *  output_jsonType - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getValue(const char * input_string, const int input_startIndex, int * output_endIndex, int * output_jsonType);

/*
 * Function: json_getObjectValueByKey
 *
 * Get object value by key with value start & end index and JSON type.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  input_key - the key of the value.
 *  input_keyStartIndex - the start index of key.
 *  input_keyEndIndex - the end index of key.
 *  output_valueStartIndex - the integer pointer.
 *   -1 - failure
 *  output_valueEndIndex - the integer pointer.
 *   -1 - failure
 *  output_valueJsonType - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getObjectValueByKey(const char * input_string, const int input_startIndex, const char * input_key, const int input_keyStartIndex, const int input_keyEndIndex, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType);

/*
 * Function: json_getArrayValueByPosition
 *
 * Get array value by position with value start & end index and JSON type.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  input_position - the value position in array.
 *  output_valueStartIndex - the integer pointer.
 *   -1 - failure
 *  output_valueEndIndex - the integer pointer.
 *   -1 - failure
 *  output_valueJsonType - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getArrayValueByPosition(const char * input_string, const int input_startIndex, const int input_position, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType);

/*
 * Function: json_getNumber
 *
 * Get the number with end index.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  output_endIndex - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getNumber(const char * input_string, const int input_startIndex, int * output_endIndex);

/*
 * Function: json_getString
 *
 * Get the string with end index.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  output_endIndex - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getString(const char * input_string, const int input_startIndex, int * output_endIndex);

/*
 * Function: json_getBoolean
 *
 * Get the boolean with end index.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  output_endIndex - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getBoolean(const char * input_string, const int input_startIndex, int * output_endIndex);

/*
 * Function: json_getNull
 *
 * Get the null with end index.
 *
 * Parameters:
 *  input_string - the character pointer.
 *  input_startIndex - the start index of the string.
 *  output_endIndex - the integer pointer.
 *   -1 - failure
 *
 * Returns:
 *   0 - success
 *  -1 - failure
 */
int json_getNull(const char * input_string, const int input_startIndex, int * output_endIndex);

#endif
