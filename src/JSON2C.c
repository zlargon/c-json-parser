#include <stdio.h>
#include "JSON2C.h"

// JSON value type description
const char * json_valueTypeDescription(JsonValueType type) {
    switch (type) {
        case JSON_VALUE_TYPE_OBJECT:  return "object";
        case JSON_VALUE_TYPE_ARRAY:   return "array";
        case JSON_VALUE_TYPE_NUMBER:  return "number";
        case JSON_VALUE_TYPE_STRING:  return "string";
        case JSON_VALUE_TYPE_BOOLEAN: return "boolean";
        case JSON_VALUE_TYPE_NULL:    return "null";
        default:
            printf("error: unknown type (%d)\n", type);
            return "unknown";
    }
}
