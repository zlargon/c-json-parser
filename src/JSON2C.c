#include <stdio.h>
#include <ctype.h>
#include "JSON2C.h"

// Internal Function
int json_getShallowObject(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getShallowArray(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getKeyValuePair(const char * input_string, const int input_startIndex, int * output_keyStartIndex, int * output_keyEndIndex, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType);
int json_getKey(const char * input_string, const int input_startIndex, int * output_keyStartIndex, int * output_keyEndIndex, int * output_keyJsonType);
int json_getValue(const char * input_string, const int input_startIndex, int * output_endIndex, int * output_jsonType);
int json_getNumber(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getString(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getBoolean(const char * input_string, const int input_startIndex, int * output_endIndex);
int json_getNull(const char * input_string, const int input_startIndex, int * output_endIndex);


// Utility Function
int json_util_printSubstring(const char * string, const int startIndex, const int endIndex);
int json_util_stringCompare(const char * s1, const int s1_startIndex, const int s1_endIndex, const char * s2, const int s2_startIndex, const int s2_endIndex);
int json_util_getNextCharacter(const char * input_string, int * index);

// JSON type description
const char * json_type_toString(int type) {
    switch (type) {
        case JSON_TYPE_OBJECT:  return "object";
        case JSON_TYPE_ARRAY:   return "array";
        case JSON_TYPE_NUMBER:  return "number";
        case JSON_TYPE_STRING:  return "string";
        case JSON_TYPE_BOOLEAN: return "boolean";
        case JSON_TYPE_NULL:    return "null";
        default:
            printf("error: unknown type (%d)\n", type);
            return "unknown";
    }
}

// Get the value with start & end index and JSON type.
int json_getValueByJS(const char * input_string, const int input_startIndex, const char * input_keys, const int input_keyStartIndex, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
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

    // set output to default
    *output_valueStartIndex = -1;
    *output_valueEndIndex   = -1;
    *output_valueJsonType   = -1;

    int i = input_startIndex;
    int key_i = input_keyStartIndex;
    for (;;) {
        // 1. get key
        int keyStartIndex, keyEndIndex, keyJsonType;
        if (json_getKey(input_keys, key_i, &keyStartIndex, &keyEndIndex, &keyJsonType) != 0) {
            if (DEBUG) {
                printf("%s: get key at %d failure\n", __func__, key_i);
            }
            return -1;
        }

        // 2-1. json object get value by key
        int valueStartIndex, valueEndIndex, valueJsonType;
        if (keyJsonType == JSON_TYPE_STRING) {
            if (json_object_getValueByKey(input_string, i, input_keys, keyStartIndex, keyEndIndex, &valueStartIndex, &valueEndIndex, &valueJsonType) != 0) {
                if (DEBUG) {
                    printf("%s: ", __func__);
                    json_util_printSubstring(input_keys, keyStartIndex, keyEndIndex);
                    printf(" is not found\n");
                }
                return -1;
            }

            // key is found
            if (DEBUG) {
                printf("%s: ", __func__);
                json_util_printSubstring(input_keys, keyStartIndex, keyEndIndex);
                printf(" = ");
                json_util_printSubstring(input_string, valueStartIndex, valueEndIndex);
                printf(" (%s)\n", json_type_toString(valueJsonType));
            }
        }

        // 2-2. json array get value by position
        else if (keyJsonType == JSON_TYPE_NUMBER) {
            // convert key string to integer position
            int j, position = 0;
            for (j = keyStartIndex; j <= keyEndIndex; j++) {
                position = position * 10 + (input_keys[j] - 48);
            }

            if (json_array_getValueByPosition(input_string, i, position, &valueStartIndex, &valueEndIndex, &valueJsonType) != 0) {
                if (DEBUG) {
                    printf("%s: ", __func__);
                    json_util_printSubstring(input_keys, keyStartIndex, keyEndIndex);
                    printf(" is not found\n");
                }
                return -1;
            }

            // key is found
            if (DEBUG) {
                printf("%s: ", __func__);
                json_util_printSubstring(input_keys, keyStartIndex, keyEndIndex);
                printf(" = ");
                json_util_printSubstring(input_string, valueStartIndex, valueEndIndex);
                printf(" (%s)\n", json_type_toString(valueJsonType));
            }
        }

        // 2-3. others, this might be BUG
        else {
            printf("%s: [BUG] key type (%s) shoud be a string or integer\n", __func__, json_type_toString(valueJsonType));
        }

        // 3. move to next key, and next value
        i = valueStartIndex;
        key_i = keyEndIndex + 2;

        if (input_keys[key_i] == '\0') {
            if (DEBUG) {
                printf("%s: it's the end of the keys (%d)\n", __func__, key_i);
            }

            *output_valueStartIndex = valueStartIndex;
            *output_valueEndIndex   = valueEndIndex;
            *output_valueJsonType   = valueJsonType;
            return 0;
        }
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
        *output_jsonType = JSON_TYPE_STRING;
        return 0;
    }

    // 2. Number
    if (json_getNumber(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_NUMBER;
        return 0;
    }

    // 3. Boolean
    if (json_getBoolean(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_BOOLEAN;
        return 0;
    }

    // 4. Null
    if (json_getNull(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_NULL;
        return 0;
    }

    // 5. Shallow Object: only find the left and right curly bracket
    if (json_getShallowObject(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_OBJECT;
        return 0;
    }

    // 6. Shallow Array: only find the left and right square bracket
    if (json_getShallowArray(input_string, input_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_ARRAY;
        return 0;
    }

    // get value failure
    *output_jsonType = -1;
    *output_endIndex = -1;
    return -1;
}

// Get value by key with value start & end index and JSON type.
int json_object_getValueByKey(const char * input_string, const int input_string_startIndex, const char * input_key, const int input_key_startIndex, const int input_key_endIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (input_key == NULL) {
        printf("%s: input_key should not be NULL\n", __func__);
        return -1;
    }

    if (input_key_startIndex < 0) {
        printf("%s: input_key_startIndex (%d) should not be negative\n", __func__, input_key_startIndex);
        return -1;
    }

    if (input_key_endIndex < input_key_startIndex) {
        printf("%s: input_key_endIndex (%d) should greater than input_key_startIndex (%d)\n", __func__, input_key_endIndex, input_key_startIndex);
        return -1;
    }

    if (output_value_startIndex == NULL) {
        printf("%s: output_value_startIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_value_endIndex == NULL) {
        printf("%s: output_value_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_value_jsonType == NULL) {
        printf("%s: output_value_jsonType should not be NULL\n", __func__);
        return -1;
    }

    // set to default
    *output_value_startIndex = -1;
    *output_value_endIndex   = -1;
    *output_value_jsonType   = -1;

    int i = input_string_startIndex;

    // check the first character
    if (input_string[i] != '{') {
        if (DEBUG) {
            printf("%s: invalid character at %d (%c, 0x%02x), it should be left curly bracket\n", __func__, i, input_string[i], input_string[i]);
        }
        return -1;
    }
    i++;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
        goto invalid_character;
    }

    // check right curly bracket
    if (input_string[i] == '}') {
        goto end_of_object;
    }

    for (;;) {
        // 1-1. get the key value pair
        int keyStartIndex, keyEndIndex, valueStartIndex, valueEndIndex, valueJsonType;
        if (json_getKeyValuePair(input_string, i, &keyStartIndex, &keyEndIndex, &valueStartIndex, &valueEndIndex, &valueJsonType) == -1) {
            if (DEBUG) {
                printf("%s: invalid JSON Key Value Pair at %d (%c)\n", __func__, i, input_string[i]);
            }
            return -1;
        }

        // 1-2. check the key (string compare)
        if (json_util_stringCompare(input_key, input_key_startIndex, input_key_endIndex, input_string, keyStartIndex, keyEndIndex) == 0) {
            // the key is found, return the value
            *output_value_startIndex = valueStartIndex;
            *output_value_endIndex   = valueEndIndex;
            *output_value_jsonType   = valueJsonType;
            return 0;
        }

        // 1-3. move to the index behind the value
        i = valueEndIndex + 1;

        // filter the blank, util find the next character
        if (json_util_getNextCharacter(input_string, &i) != 0) {
            goto invalid_character;
        }

        // 2. check the character after the VALUE
        switch (input_string[i]) {
            // 2-1. check right square bracket
            case '}':
                goto end_of_object;

            // 2-2. find comma behind the value
            case ',':
                i++;
                break;

            // 2-3. is not right square bracket or comma
            default:
                goto invalid_character;
        }

        // filter the blank, util find the next character
        if (json_util_getNextCharacter(input_string, &i) != 0) {
            goto invalid_character;
        }
    }

invalid_character:
    if (DEBUG) {
        printf("%s: invalid character at %d (%c 0x%02x)\n", __func__, i, input_string[i], input_string[i]);
    }
    return -1;

end_of_object:
    if (DEBUG) {
        printf("%s: it's the end of the object (%d), ", __func__, i);
        json_util_printSubstring(input_key, input_key_startIndex, input_key_endIndex);
        printf(" is not found\n");
    }
    return -1;
}

// Get array value by position with value start & end index and JSON type.
int json_array_getValueByPosition(const char * input_string, const int input_string_startIndex, const int input_array_position, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType) {
    const char DEBUG = 0;

    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (input_array_position < 0) {
        printf("%s: input_array_position (%d) should not be negative\n", __func__, input_array_position);
        return -1;
    }

    if (output_value_startIndex == NULL) {
        printf("%s: output_value_startIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_value_endIndex == NULL) {
        printf("%s: output_value_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_value_jsonType == NULL) {
        printf("%s: output_value_jsonType should not be NULL\n", __func__);
        return -1;
    }

    // set to default
    *output_value_startIndex = -1;
    *output_value_endIndex = -1;
    *output_value_jsonType = -1;

    int i = input_string_startIndex;

    // check the first character
    if (input_string[i] != '[') {
        if (DEBUG) {
            printf("%s: invalid character at %d (%c, 0x%02x), it should be left square bracket\n", __func__, i, input_string[i], input_string[i]);
        }
        return -1;
    }
    i++;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
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
        if (json_getValue(input_string, i, &endIndex, &jsonType) != 0) {
            if (DEBUG) {
                printf("%s: invalid JSON Value at %d (%c)\n", __func__, i, input_string[i]);
            }
            return -1;
        }

        // 1-2. check the position
        if (input_array_position == ++position) {
            // the value is found
            *output_value_startIndex = i;
            *output_value_endIndex = endIndex;
            *output_value_jsonType = jsonType;
            return 0;
        }

        // 1-3. move to the index behind the value
        i = endIndex + 1;


        // filter the blank, util find the next character
        if (json_util_getNextCharacter(input_string, &i) != 0) {
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
        if (json_util_getNextCharacter(input_string, &i) != 0) {
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
        printf("%s: it's the end of the array (%d), the Value Array[%d] is not found\n", __func__, i, input_array_position);
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

int json_util_getNextCharacter(const char * input_string, int * index) {
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

int json_util_printSubstring(const char * string, const int startIndex, const int endIndex) {
    if (string == NULL) {
        printf("%s: string should not be NULL\n", __func__);
        return -1;
    }

    if (startIndex < 0) {
        printf("%s: startIndex (%d) should not be negative\n", __func__, startIndex);
        return -1;
    }

    if (startIndex > endIndex) {
        printf("%s: endIndex (%d) should greater than startIndex (%d)\n", __func__, endIndex, startIndex);
        return -1;
    }

    int i;
    for (i = startIndex; i <= endIndex; i++) {
        printf("%c", string[i]);
    }
    return 0;
}

int json_util_stringCompare(const char * s1, const int s1_startIndex, const int s1_endIndex, const char * s2, const int s2_startIndex, const int s2_endIndex) {
    const char DEBUG = 0;

    // check arguments
    if (s1 == NULL) {
        printf("%s: s1 should not be NULL\n", __func__);
        return -1;
    }

    if (s1_startIndex < 0) {
        printf("%s: s1_startIndex (%d) should not be negative\n", __func__, s1_startIndex);
        return -1;
    }

    if (s1_endIndex < s1_startIndex) {
        printf("%s: s1_endIndex (%d) should greater than s1_startIndex (%d)\n", __func__, s1_endIndex, s1_startIndex);
        return -1;
    }

    if (s2 == NULL) {
        printf("%s: s2 should not be NULL\n", __func__);
        return -1;
    }

    if (s2_startIndex < 0) {
        printf("%s: s2_startIndex (%d) should not be negative\n", __func__, s2_startIndex);
        return -1;
    }

    if (s2_endIndex < s2_startIndex) {
        printf("%s: s2_endIndex (%d) should greater than s2_startIndex (%d)\n", __func__, s2_endIndex, s2_startIndex);
        return -1;
    }

    // check string length
    int s1_length = s1_endIndex - s1_startIndex + 1;
    int s2_length = s2_endIndex - s2_startIndex + 1;
    if (s1_length != s2_length) {
        if (DEBUG) {
            printf("%s: string length is not match, s1_length = %d, s2_length = %d\n\n", __func__, s1_length, s2_length);
        }
        return -1;
    }

    int i;
    for (i = 0; i < s1_length; i++) {
        if (s1[s1_startIndex + i] != s2[s2_startIndex + i]) {
            if (DEBUG) {
                printf("%s: string is not match, s1[%d] = %c, s2[%d] = %c\n\n", __func__, s1_startIndex + i, s1[s1_startIndex + i], s2_startIndex + i, s2[s2_startIndex + i]);
            }
            return -1;
        }
    }

    if (DEBUG) {
        printf("%s: string is match\n", __func__);
        printf("   s1[%d..%d] = ", s1_startIndex, s1_endIndex);
        json_util_printSubstring(s1, s1_startIndex, s1_endIndex);

        printf("\n   s2[%d..%d] = ", s2_startIndex, s2_endIndex);
        json_util_printSubstring(s2, s2_startIndex, s2_endIndex);
        puts("\n");
    }
    return 0;
}

int json_getKeyValuePair(const char * input_string, const int input_startIndex, int * output_keyStartIndex, int * output_keyEndIndex, int * output_valueStartIndex, int * output_valueEndIndex, int * output_valueJsonType) {
    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_keyStartIndex == NULL) {
        printf("%s: output_keyStartIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_keyEndIndex == NULL) {
        printf("%s: output_keyEndIndex should not be NULL\n", __func__);
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

    // set output parameters to default -1
    *output_keyStartIndex   = -1;
    *output_keyEndIndex     = -1;
    *output_valueStartIndex = -1;
    *output_valueEndIndex   = -1;
    *output_valueJsonType   = -1;

    int i = input_startIndex;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
        return -1;
    }

    // 1. get key success
    int keyStartIndex = i;
    int keyEndIndex;
    if (json_getString(input_string, keyStartIndex, &keyEndIndex) != 0) {
        return -1;
    }

    // move to the index behind the key
    i = keyEndIndex + 1;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
        return -1;
    }

    // 2. find the colon
    if (input_string[i] != ':') {
        printf("%s: the character at %d (%c 0x%02x) should be colon (:)\n", __func__, i, input_string[i], input_string[i]);
        return -1;
    }
    i++;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
        return -1;
    }

    // 3. get value
    int valueStartIndex = i;
    int valueEndIndex;
    int valueJsonType;
    if (json_getValue(input_string, valueStartIndex, &valueEndIndex, &valueJsonType) != 0) {
        return -1;
    }

    *output_keyStartIndex   = keyStartIndex;
    *output_keyEndIndex     = keyEndIndex;
    *output_valueStartIndex = valueStartIndex;
    *output_valueEndIndex   = valueEndIndex;
    *output_valueJsonType   = valueJsonType;
    return 0;
}

int json_getKey(const char * input_string, const int input_startIndex, int * output_keyStartIndex, int * output_keyEndIndex, int * output_keyJsonType) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_startIndex < 0) {
        printf("%s: input_startIndex (%d) should not be negative\n", __func__, input_startIndex);
        return -1;
    }

    if (output_keyStartIndex == NULL) {
        printf("%s: output_keyStartIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_keyEndIndex == NULL) {
        printf("%s: output_keyEndIndex should not be NULL\n", __func__);
        return -1;
    }

    *output_keyStartIndex = -1;
    *output_keyEndIndex   = -1;
    *output_keyJsonType   = -1;

    int endIndex;
    if (json_getShallowArray(input_string, input_startIndex, &endIndex) != 0) {
        return -1;
    }

    int keyStartIndex = input_startIndex + 1;
    int keyEndIndex   = endIndex - 1;

    // check the key length
    if (keyEndIndex < keyStartIndex) {
        if (DEBUG) {
            printf("%s: the key is empty\n", __func__);
        }
        return -1;
    }

    // 1. string key
    if (json_getString(input_string, input_startIndex + 1, &endIndex) == 0 && keyEndIndex == endIndex) {
        *output_keyStartIndex = keyStartIndex;
        *output_keyEndIndex   = keyEndIndex;
        *output_keyJsonType   = JSON_TYPE_STRING;

        if (DEBUG) {
            printf("%s: ", __func__);
            json_util_printSubstring(input_string, *output_keyStartIndex, *output_keyEndIndex);
            printf(" (%s)\n", json_type_toString(*output_keyJsonType));
        }
        return 0;
    }

    // 2. number key
    int i;
    for (i = keyStartIndex; i <= keyEndIndex; i++) {
        if (!isdigit(input_string[i])) {
            return -1;
        }
    }

    *output_keyStartIndex = keyStartIndex;
    *output_keyEndIndex   = keyEndIndex;
    *output_keyJsonType   = JSON_TYPE_NUMBER;

    if (DEBUG) {
        printf("%s: ", __func__);
        json_util_printSubstring(input_string, *output_keyStartIndex, *output_keyEndIndex);
        printf(" (%s)\n", json_type_toString(*output_keyJsonType));
    }
    return 0;
}
