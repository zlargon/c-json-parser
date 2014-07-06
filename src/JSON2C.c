#include <stdio.h>
#include <ctype.h>
#include "JSON2C.h"

// Internal Function
int json_getShallowObject(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getShallowArray(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getNextCharacterWithoutBlank(const char * input_string, int * index);


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

// Get the value with end index and JSON type.
int json_getValue(const char * input_string, const int input_startIndex, int * output_endIndex, int * output_jsonType) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_jsonType == NULL) {
        printf("%s: output_jsonType should not be NULL\n", __func__);
        return -1;
    }


    // 1. String
    if (json_getString(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_STRING;
        return 0;
    }

    // 2. Number
    if (json_getNumber(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_NUMBER;
        return 0;
    }

    // 3. Boolean
    if (json_getBoolean(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_BOOLEAN;
        return 0;
    }

    // 4. Null
    if (json_getNull(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_NULL;
        return 0;
    }

    // 5. Shallow Object: only find the left and right curly bracket
    if (json_getShallowObject(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_OBJECT;
        return 0;
    }

    // 6. Shallow Array: only find the left and right square bracket
    if (json_getShallowArray(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_VALUE_TYPE_ARRAY;
        return 0;
    }

    // get value failure
    *output_jsonType = -1;
    *output_endIndex = -1;
    return -1;
}

// Get array value by position with value start & end index and JSON type.
int json_getArrayValueByPosition(const char * input_string, const int input_startIndex, const int input_position, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (input_position < 0) {
        printf("%s: input_position (%d) should not be negative\n", __func__, input_position);
        return -1;
    }

    if (output_valueStartIndex == NULL) {
        printf("%s: output_valueStartIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_valueEndIndex == NULL) {
        printf("%s: output_valueEndIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_valueJsonType == NULL) {
        printf("%s: output_valueJsonType should not be NULL\n", __func__);
        return -1;
    }

    // set to default
    *output_valueStartIndex = -1;
    *output_valueEndIndex = -1;
    *output_valueJsonType = -1;

    int i = input_startIndex;

    // check the first character
    if (input_string[i] != '[') {
        if (DEBUG) {
            printf("%s: invalid character at %d (%c, 0x%02x), it should be left square bracket\n", __func__, i, input_string[i], input_string[i]);
        }
        return -1;
    }
    i++;

    // filter the blank, util find the next character
    if (json_getNextCharacterWithoutBlank(input_string, &i) == -1) {
        goto invalid_character;
    }

    // check right square bracket
    if (input_string[i] == ']') {
        goto end_of_array;
    }

    int position = -1;
    for (;;) {

        // 1-1. check the value
        int endIndex, jsonType;
        if (json_getValue(input_string, i, &endIndex, &jsonType) == -1) {
            if (DEBUG) {
                printf("%s: invalid JSON Value at %d (%c)\n", __func__, i, input_string[i]);
            }
            return -1;
        }

        // 1-2. check the position
        if (input_position == ++position) {
            // the value is found
            *output_valueStartIndex = i;
            *output_valueEndIndex = endIndex;
            *output_valueJsonType = jsonType;
            return 0;
        }

        // 1-3. move to the index behind the value
        i = endIndex + 1;


        // filter the blank, util find the next character
        if (json_getNextCharacterWithoutBlank(input_string, &i) == -1) {
            goto invalid_character;
        }

        // 2. check the character after the VALUE
        switch (input_string[i]) {
            // 2-1. check right square bracket
            case ']':
                goto end_of_array;

            // 2-2. find comma behind the value
            case ',':
                i++;
                break;

            // 2-3. is not right square bracket or comma
            default:
                goto invalid_character;
        }

        // filter the blank, util find the next character
        if (json_getNextCharacterWithoutBlank(input_string, &i) == -1) {
            goto invalid_character;
        }
    }

invalid_character:
    if (DEBUG) {
        printf("%s: invalid character at %d (%c 0x%02x)\n", __func__, i, input_string[i], input_string[i]);
    }
    return -1;

end_of_array:
    if (DEBUG) {
        printf("%s: it's the end of the array (%d), the Value Array[%d] is not found\n", __func__, i, input_position);
    }
    return -1;
}

// Get the number with end index
int json_getNumber(const char * input_string, const int input_startIndex, int * output_endIndex) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    int i = input_startIndex;
    *output_endIndex = -1; // set default to -1

    // check minus sign
    if (input_string[i] == '-') {
        i++;
    }

integer_part:
    // check integer part first digist
    if (input_string[i] == '0') {
        i++;
        goto fractional_part;
    }

    if (!isdigit(input_string[i])) {
        if (DEBUG) {
            printf("%s: the character at %d (%c 0x%02x) should be a digist\n", __func__, i, input_string[i], input_string[i]);
        }
        return -1;
    }

    // check the other characters behind the first one
    while (isdigit(input_string[++i]));

fractional_part:
    // check '.'
    if (input_string[i] != '.') {
        goto exponent_part;
    }

    // check the first character behind the '.'
    if (!isdigit(input_string[i + 1])) {
        goto exponent_part;
    }

    while (isdigit(input_string[++i]));

exponent_part:
    // 1. check e / E
    if (input_string[i] != 'e' && input_string[i] != 'E') {
        *output_endIndex = i - 1;
        return 0;
    }

    // 2. check + / -
    if (input_string[i + 1] == '+' || input_string[i + 1] == '-') {
        i++;
    }

    // 3. check the first character behind the e, E, +, -
    if (!isdigit(input_string[i + 1])) {

        if (input_string[i] == 'e' || input_string[i] == 'E') {
            *output_endIndex = i - 1;
            return 0;
        }

        if (input_string[i] == '+' || input_string[i] == '-') {
            *output_endIndex = i - 2;
            return 0;
        }

        printf("[BUG] %s: please report the log to project owner, thanks.\n", __func__);
        puts("================================================================================");
        printf("input_string = %s\n", input_string);
        printf("input_startIndex = %d\n", input_startIndex);
        puts("================================================================================\n");
        return -1;
    }

    while (isdigit(input_string[++i]));

    // success
    *output_endIndex = i - 1;
    return 0;
}

// Get the string with end index
int json_getString(const char * input_string, const int input_startIndex, int * output_endIndex) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    int i = input_startIndex;
    *output_endIndex = -1; // set default to -1

    // check first character
    if (input_string[i] != '\"') {
        if (DEBUG) {
            printf("%s: the first character (%c 0x%02x) should be quotation mark (\")\n", __func__, input_string[i], input_string[i]);
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
                    printf("%s: is the end of string at %d\n", __func__, i);
                }
            } else {
                if (DEBUG) {
                    printf("%s: invalid control character at %d (0x%02x)\n", __func__, i, input_string[i]);
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
                                printf("%s: non-hexadecimal digit character behind \\u at %d (%c 0x%02x)\n", __func__, i, input_string[i], input_string[i]);
                            }
                            return -1;
                        }
                    }
                    break;
                }

                default:
                    if (DEBUG) {
                        printf("%s: invalid character behind the reverse solidus at %d (%c, 0x%02x)\n", __func__, i, input_string[i], input_string[i]);
                    }
                    return -1;
            }
        }

        // 4. other character
        i++;
    }
}

// Get the boolean with end index.
int json_getBoolean(const char * input_string, const int input_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    *output_endIndex = -1; // set default to -1

    // check the first character
    const char * booleanValue = input_string[input_startIndex] == 't' ? "true" : (input_string[input_startIndex] == 'f' ? "false" : NULL);
    if (booleanValue == NULL) {
        return -1;
    }

    // string compare
    int i;
    for (i = 0; booleanValue[i] != '\0'; i++) {
        if (booleanValue[i] != input_string[input_startIndex + i]) {
            return -1;
        }
    }

    // success
    *output_endIndex = input_startIndex + i - 1;
    return 0;
}

// Get the null with end index.
int json_getNull(const char * input_string, const int input_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    *output_endIndex = -1; // set default to -1

    // check the first character
    const char * nullValue = "null";

    // string compare
    int i;
    for (i = 0; nullValue[i] != '\0'; i++) {
        if (nullValue[i] != input_string[input_startIndex + i]) {
            return -1;
        }
    }

    // success
    *output_endIndex = input_startIndex + i - 1;
    return 0;
}



/* Internal Function */

int json_getShallowObject(const char * input_string, const int input_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (input_string[input_startIndex] == '{') {

        int i, stack = 1;
        for (i = input_startIndex + 1; input_string[i] != '\0'; i++) {

            // left curly bracket
            if (input_string[i] == '{') {
                stack++;
                continue;
            }

            // right curly bracket
            if (input_string[i] == '}') {
                stack--;

                if (stack == 0) {
                    *output_endIndex = i;
                    return 0;
                }
            }
        }
    }

    *output_endIndex = -1;
    return -1;
}

int json_getShallowArray(const char * input_string, const int input_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (input_string[input_startIndex] == '[') {

        int i, stack = 1;
        for (i = input_startIndex + 1; input_string[i] != '\0'; i++) {

            // left square bracket
            if (input_string[i] == '[') {
                stack++;
                continue;
            }

            // right square bracket
            if (input_string[i] == ']') {
                stack--;

                if (stack == 0) {
                    *output_endIndex = i;
                    return 0;
                }
            }
        }
    }

    *output_endIndex = -1;
    return -1;
}

int json_getNextCharacterWithoutBlank(const char * input_string, int * index) {
    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (index == NULL) {
        printf("%s: index should not be NULL\n", __func__);
        return -1;
    }

    if (*index < 0) {
        printf("%s: *index (%d) should not be negative\n", __func__, *index);
        return -1;
    }

    while (!isprint(input_string[*index]) || isspace(input_string[*index])) {
        if (input_string[*index] == '\0') {
            // it's the end of the string
            return -1;
        }
        (*index)++;
    }

    return 0;
}
