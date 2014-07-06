#include <stdio.h>
#include <ctype.h>
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

// Get the string with end index
int json_getString(const char * input_string, const int input_startIndex, int * output_endIndex) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("json_getString: input_string should not be NULL\n");
        return -1;
    }

    if (input_startIndex < 0) {
        printf("json_getString: input_startIndex (%d) should not be negative\n", input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("json_getString: output_endIndex should not be NULL\n");
        return -1;
    }

    int i = input_startIndex;
    *output_endIndex = -1; // set default to -1

    // check first character
    if (input_string[i] != '\"') {
        if (DEBUG) {
            printf("json_getString: the first character (%c 0x%02x) should be quotation mark (\")\n", input_string[i], input_string[i]);
        }
        return -1;
    }
    i++;

    for (;;) {
        // 1. check quotation mark
        if (input_string[i] == '\"') {
            *output_endIndex = i;
            return 0;
        }

        // 2. check control character
        if (iscntrl(input_string[i])) {
            if (input_string[i] == '\0') {
                if (DEBUG) {
                    printf("json_getString: is the end of string at %d\n", i);
                }
            } else {
                if (DEBUG) {
                    printf("json_getString: invalid control character at %d (0x%02x)\n", i, input_string[i]);
                }
            }
            return -1;
        }

        // 3. check reverse solidus
        if (input_string[i] == '\\') {
            i++;

            // check the characters behind the reverse solidus
            switch (input_string[i]) {
                case '\"':  // quotation mark
                case '\\':  // reverse solidus
                case '/':   // solidus
                case 'b':   // backspace
                case 'f':   // form feed
                case 'n':   // line feed
                case 'r':   // carriage return
                case 't':   // character tabulation
                    break;

                // Unicode
                case 'u': {
                    // check 4 hexadecimal digits behind \u
                    int j;
                    for (j = 0; j < 4; j++) {
                        if (!isxdigit(input_string[++i])) {
                            if (DEBUG) {
                                printf("json_getString: non-hexadecimal digit character behind \\u at %d (%c 0x%02x)\n", i, input_string[i], input_string[i]);
                            }
                            return -1;
                        }
                    }
                    break;
                }

                default:
                    if (DEBUG) {
                        printf("json_getString: invalid character behind the reverse solidus at %d (%c, 0x%02x)\n", i, input_string[i], input_string[i]);
                    }
                    return -1;
            }
        }

        // 4. other character
        i++;
    }
}
