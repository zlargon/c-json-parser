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

#endif
