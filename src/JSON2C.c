#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "JSON2C.h"

// 1. JSON API
const char * json_type_toString(int type);
int             json_getValueByJS(const char * input_string, const int input_string_startIndex, const char * input_keys, const int input_keys_startIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);
int     json_object_getValueByKey(const char * input_string, const int input_string_startIndex, const char * input_key, const int input_key_startIndex, const int input_key_endIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);
int json_array_getValueByPosition(const char * input_string, const int input_string_startIndex, const int input_array_position, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);
int      json_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size);
int json_keyValuePair_free(JSON_Key_Value_Pair * keyValuePair);

// 2. Internal Function
int json_object_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size);
int  json_array_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size);
int            json_getKeyValuePair(const char * input_string, const int input_string_startIndex, int * output_key_startIndex, int * output_key_endIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType);
int                     json_getKey(const char * input_string, const int input_string_startIndex, int * output_key_startIndex, int * output_key_endIndex, int * output_key_jsonType);

// 3. Get Value Function
int           json_getValue(const char * input_string, const int input_string_startIndex, int * output_endIndex, int * output_jsonType);
int json_getObjectInShallow(const char * input_string, const int input_string_startIndex, int * output_endIndex);
int  json_getArrayInShallow(const char * input_string, const int input_string_startIndex, int * output_endIndex);
int          json_getNumber(const char * input_string, const int input_string_startIndex, int * output_endIndex);
int          json_getString(const char * input_string, const int input_string_startIndex, int * output_endIndex);
int         json_getBoolean(const char * input_string, const int input_string_startIndex, int * output_endIndex);
int            json_getNull(const char * input_string, const int input_string_startIndex, int * output_endIndex);

// 4. Utility Function
int json_util_getNextCharacter(const char * string, int * index);
int json_util_printSubstring(const char * string, const int startIndex, const int endIndex);
int json_util_allocSubstring(const char * string, const int startIndex, const int endIndex, char ** substring);
int json_util_allocStringByInteger(const int number, char ** string);
int json_util_stringCompare(const char * s1, const int s1_startIndex, const int s1_endIndex, const char * s2, const int s2_startIndex, const int s2_endIndex);


// 1-1. JSON type description
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

// 1-2. Get the value with start & end index and JSON type.
int json_getValueByJS(const char * input_string, const int input_string_startIndex, const char * input_keys, const int input_keys_startIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (input_keys == NULL) {
        printf("%s: input_keys should not be NULL\n", __func__);
        return -1;
    }

    if (input_keys_startIndex < 0) {
        printf("%s: input_keys_startIndex (%d) should not be negative\n", __func__, input_keys_startIndex);
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

    // set output to default
    *output_value_startIndex = -1;
    *output_value_endIndex   = -1;
    *output_value_jsonType   = -1;

    int i = input_string_startIndex;
    int key_i = input_keys_startIndex;

    int key_startIndex, key_endIndex, key_jsonType;
    int value_startIndex, value_endIndex, value_jsonType;

get_value_by_js_loop:

    // 1. get key
    if (json_getKey(input_keys, key_i, &key_startIndex, &key_endIndex, &key_jsonType) != 0) {
        if (DEBUG) {
            printf("%s: get key at %d failure\n", __func__, key_i);
        }
        return -1;
    }

    // 2-1. json object get value by key
    if (key_jsonType == JSON_TYPE_STRING) {
        if (json_object_getValueByKey(input_string, i, input_keys, key_startIndex, key_endIndex, &value_startIndex, &value_endIndex, &value_jsonType) != 0) {
            if (DEBUG) {
                printf("%s: ", __func__);
                json_util_printSubstring(input_keys, key_startIndex, key_endIndex);
                printf(" is not found\n");
            }
            return -1;
        }

        // key is found
        if (DEBUG) {
            printf("%s: ", __func__);
            json_util_printSubstring(input_keys, key_startIndex, key_endIndex);
            printf(" = ");
            json_util_printSubstring(input_string, value_startIndex, value_endIndex);
            printf(" (%s)\n", json_type_toString(value_jsonType));
        }
    }

    // 2-2. json array get value by position
    else if (key_jsonType == JSON_TYPE_NUMBER) {
        // convert key string to integer position
        int j, position = 0;
        for (j = key_startIndex; j <= key_endIndex; j++) {
            position = position * 10 + (input_keys[j] - 48);
        }

        if (json_array_getValueByPosition(input_string, i, position, &value_startIndex, &value_endIndex, &value_jsonType) != 0) {
            if (DEBUG) {
                printf("%s: ", __func__);
                json_util_printSubstring(input_keys, key_startIndex, key_endIndex);
                printf(" is not found\n");
            }
            return -1;
        }

        // key is found
        if (DEBUG) {
            printf("%s: ", __func__);
            json_util_printSubstring(input_keys, key_startIndex, key_endIndex);
            printf(" = ");
            json_util_printSubstring(input_string, value_startIndex, value_endIndex);
            printf(" (%s)\n", json_type_toString(value_jsonType));
        }
    }

    // 2-3. others, this might be BUG
    else {
        printf("%s: [BUG] key type (%s) shoud be a string or integer\n", __func__, json_type_toString(value_jsonType));
    }

    // 3. move to next key, and next value
    i = value_startIndex;
    key_i = key_endIndex + 2;

    if (input_keys[key_i] == '\0') {
        if (DEBUG) {
            printf("%s: it's the end of the keys (%d)\n", __func__, key_i);
        }

        *output_value_startIndex = value_startIndex;
        *output_value_endIndex   = value_endIndex;
        *output_value_jsonType   = value_jsonType;
        return 0;
    }

    goto get_value_by_js_loop;
}

// 1-3. Get value by key with value start & end index and JSON type.
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

    int key_startIndex, key_endIndex;
    int value_startIndex, value_endIndex, value_jsonType;

get_key_value_pair_loop:
    // 1-1. get the key value pair
    if (json_getKeyValuePair(input_string, i, &key_startIndex, &key_endIndex, &value_startIndex, &value_endIndex, &value_jsonType) != 0) {
        if (DEBUG) {
            printf("%s: invalid JSON Key Value Pair at %d (%c)\n", __func__, i, input_string[i]);
        }
        return -1;
    }

    // 1-2. check the key (string compare)
    if (json_util_stringCompare(input_key, input_key_startIndex, input_key_endIndex, input_string, key_startIndex, key_endIndex) == 0) {
        // the key is found, return the value
        *output_value_startIndex = value_startIndex;
        *output_value_endIndex   = value_endIndex;
        *output_value_jsonType   = value_jsonType;
        return 0;
    }

    // 1-3. move to the index behind the value
    i = value_endIndex + 1;

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

    goto get_key_value_pair_loop;

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

// 1-4. Get array value by position with value start & end index and JSON type.
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
    int endIndex, jsonType;

get_value_by_position_loop:
        // 1-1. check the value
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

    goto get_value_by_position_loop;

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

// 1-5. object or array get key value pair list
int json_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size) {
    if (json_object_getKeyValuePairList(input_string, input_string_startIndex, output_keyValuePairList, output_keyValuePairList_size) == 0) {
        return 0;
    }

    if (json_array_getKeyValuePairList(input_string, input_string_startIndex, output_keyValuePairList, output_keyValuePairList_size) == 0) {
        return 0;
    }

    return -1;
}

// 1-6. Free JSON Key Value Pair in recursive
int json_keyValuePair_free(JSON_Key_Value_Pair * keyValuePair) {

    if (keyValuePair == NULL) {
        return 0;
    }

    json_keyValuePair_free(keyValuePair->next);
    free(keyValuePair->key);
    free(keyValuePair->value);
    free(keyValuePair);
    return 0;
}

// 2-1. Object Get Key Value Pair List
int json_object_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_keyValuePairList == NULL) {
        printf("%s: output_keyValuePairList should not be NULL\n", __func__);
        return -1;
    }

    if (output_keyValuePairList_size == NULL) {
        printf("%s: output_keyValuePairList_size should not be NULL\n", __func__);
        return -1;
    }

    int i = input_string_startIndex;

    *output_keyValuePairList = NULL;
    *output_keyValuePairList_size = 0;

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
        return 0;
    }

    JSON_Key_Value_Pair * last = *output_keyValuePairList;

    for (;;) {
        // 1. get the key value pair
        int key_startIndex, key_endIndex;
        int value_startIndex, value_endIndex, value_jsonType;
        if (json_getKeyValuePair(input_string, i, &key_startIndex, &key_endIndex, &value_startIndex, &value_endIndex, &value_jsonType) != 0) {
            if (DEBUG) {
                printf("%s: invalid JSON Key Value Pair at %d (%c)\n", __func__, i, input_string[i]);
            }
            return -1;
        }

        // 2. create JSON_Key_Value_Pair
        JSON_Key_Value_Pair * pair = malloc(sizeof(JSON_Key_Value_Pair));
        if (pair == NULL) {
            printf("%s: out of memory\n", __func__);
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // key & key_type
        pair->key_type = JSON_TYPE_STRING;
        if (json_util_allocSubstring(input_string, key_startIndex, key_endIndex, &(pair->key)) != 0) {
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // value & value_type
        pair->value_type = value_jsonType;
        if (json_util_allocSubstring(input_string, value_startIndex, value_endIndex, &(pair->value)) != 0) {
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // next pointer
        pair->next = NULL;

        // 3. add to output_keyValuePairList
        if (last == NULL) {
            *output_keyValuePairList = pair;
            last = pair;
        } else {
            last->next = pair;
            last = pair;
        }

        (*output_keyValuePairList_size)++;

        // 4. move to the index behind the value
        i = value_endIndex + 1;

        // filter the blank, util find the next character
        if (json_util_getNextCharacter(input_string, &i) != 0) {
            goto invalid_character;
        }

        // 5. check the character after the VALUE
        switch (input_string[i]) {
            // 2-1. check right square bracket
            case '}':
                // it's the end of the object
                return 0;

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
}

// 2-2. Array Get Key Value Pair List
int  json_array_getKeyValuePairList(const char * input_string, const int input_string_startIndex, JSON_Key_Value_Pair ** output_keyValuePairList, int * output_keyValuePairList_size) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_keyValuePairList == NULL) {
        printf("%s: output_keyValuePairList should not be NULL\n", __func__);
        return -1;
    }

    if (output_keyValuePairList_size == NULL) {
        printf("%s: output_keyValuePairList_size should not be NULL\n", __func__);
        return -1;
    }

    int i = input_string_startIndex;

    *output_keyValuePairList = NULL;
    *output_keyValuePairList_size = 0;

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
        return 0;
    }

    JSON_Key_Value_Pair * last = *output_keyValuePairList;
    for (;;) {
        // 1-1. check the value
        int endIndex, jsonType;
        if (json_getValue(input_string, i, &endIndex, &jsonType) != 0) {
            if (DEBUG) {
                printf("%s: invalid JSON Value at %d (%c)\n", __func__, i, input_string[i]);
            }
            return -1;
        }

        // 2. create JSON_Key_Value_Pair
        JSON_Key_Value_Pair * pair = malloc(sizeof(JSON_Key_Value_Pair));
        if (pair == NULL) {
            printf("%s: out of memory\n", __func__);
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // key & key_type
        pair->key_type = JSON_TYPE_NUMBER;
        if (json_util_allocStringByInteger(*output_keyValuePairList_size, &(pair->key)) != 0) {
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // value & value_type
        pair->value_type = jsonType;
        if (json_util_allocSubstring(input_string, i, endIndex, &(pair->value)) != 0) {
            json_keyValuePair_free(*output_keyValuePairList);
            return -1;
        }

        // next pointer
        pair->next = NULL;

        // 3. add to output_keyValuePairList
        if (last == NULL) {
            *output_keyValuePairList = pair;
            last = pair;
        } else {
            last->next = pair;
            last = pair;
        }

        (*output_keyValuePairList_size)++;

        // 4. move to the index behind the value
        i = endIndex + 1;

        // filter the blank, util find the next character
        if (json_util_getNextCharacter(input_string, &i) != 0) {
            goto invalid_character;
        }

        // 5. check the character after the VALUE
        switch (input_string[i]) {
            // 2-1. check right square bracket
            case ']':
                // it's the end of the array
                return 0;

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
}

// 2-3. Get Key Value Pair
int json_getKeyValuePair(const char * input_string, const int input_string_startIndex, int * output_key_startIndex, int * output_key_endIndex, int * output_value_startIndex, int * output_value_endIndex, int * output_value_jsonType) {
    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_key_startIndex == NULL) {
        printf("%s: output_key_startIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_key_endIndex == NULL) {
        printf("%s: output_key_endIndex should not be NULL\n", __func__);
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

    // set output parameters to default -1
    *output_key_startIndex   = -1;
    *output_key_endIndex     = -1;
    *output_value_startIndex = -1;
    *output_value_endIndex   = -1;
    *output_value_jsonType   = -1;

    int i = input_string_startIndex;

    // filter the blank, util find the next character
    if (json_util_getNextCharacter(input_string, &i) != 0) {
        return -1;
    }

    // 1. get key success
    int key_startIndex = i;
    int key_endIndex;
    if (json_getString(input_string, key_startIndex, &key_endIndex) != 0) {
        return -1;
    }

    // move to the index behind the key
    i = key_endIndex + 1;

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
    int value_startIndex = i;
    int value_endIndex;
    int value_jsonType;
    if (json_getValue(input_string, value_startIndex, &value_endIndex, &value_jsonType) != 0) {
        return -1;
    }

    *output_key_startIndex   = key_startIndex;
    *output_key_endIndex     = key_endIndex;
    *output_value_startIndex = value_startIndex;
    *output_value_endIndex   = value_endIndex;
    *output_value_jsonType   = value_jsonType;
    return 0;
}

// 2-4. Get Key
int json_getKey(const char * input_string, const int input_string_startIndex, int * output_key_startIndex, int * output_key_endIndex, int * output_key_jsonType) {
    const char DEBUG = 0;

    // check arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_key_startIndex == NULL) {
        printf("%s: output_key_startIndex should not be NULL\n", __func__);
        return -1;
    }

    if (output_key_endIndex == NULL) {
        printf("%s: output_key_endIndex should not be NULL\n", __func__);
        return -1;
    }

    *output_key_startIndex = -1;
    *output_key_endIndex   = -1;
    *output_key_jsonType   = -1;

    int endIndex;
    if (json_getArrayInShallow(input_string, input_string_startIndex, &endIndex) != 0) {
        return -1;
    }

    int key_startIndex = input_string_startIndex + 1;
    int key_endIndex   = endIndex - 1;

    // check the key length
    if (key_endIndex < key_startIndex) {
        if (DEBUG) {
            printf("%s: the key is empty\n", __func__);
        }
        return -1;
    }

    // 1. string key
    if (json_getString(input_string, input_string_startIndex + 1, &endIndex) == 0 && key_endIndex == endIndex) {
        *output_key_startIndex = key_startIndex;
        *output_key_endIndex   = key_endIndex;
        *output_key_jsonType   = JSON_TYPE_STRING;

        if (DEBUG) {
            printf("%s: ", __func__);
            json_util_printSubstring(input_string, *output_key_startIndex, *output_key_endIndex);
            printf(" (%s)\n", json_type_toString(*output_key_jsonType));
        }
        return 0;
    }

    // 2. number key
    int i;
    for (i = key_startIndex; i <= key_endIndex; i++) {
        if (!isdigit(input_string[i])) {
            return -1;
        }
    }

    *output_key_startIndex = key_startIndex;
    *output_key_endIndex   = key_endIndex;
    *output_key_jsonType   = JSON_TYPE_NUMBER;

    if (DEBUG) {
        printf("%s: ", __func__);
        json_util_printSubstring(input_string, *output_key_startIndex, *output_key_endIndex);
        printf(" (%s)\n", json_type_toString(*output_key_jsonType));
    }
    return 0;
}


// 3. Get Value with start & end index and JSON type
int json_getValue(const char * input_string, const int input_string_startIndex, int * output_endIndex, int * output_jsonType) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
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
    if (json_getString(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_STRING;
        return 0;
    }

    // 2. Number
    if (json_getNumber(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_NUMBER;
        return 0;
    }

    // 3. Boolean
    if (json_getBoolean(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_BOOLEAN;
        return 0;
    }

    // 4. Null
    if (json_getNull(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_NULL;
        return 0;
    }

    // 5. Shallow Object: only find the left and right curly bracket
    if (json_getObjectInShallow(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_OBJECT;
        return 0;
    }

    // 6. Shallow Array: only find the left and right square bracket
    if (json_getArrayInShallow(input_string, input_string_startIndex, output_endIndex) == 0) {
        *output_jsonType = JSON_TYPE_ARRAY;
        return 0;
    }

    // get value failure
    *output_jsonType = -1;
    *output_endIndex = -1;
    return -1;
}

// 3-1. Get Object in Shallow
int json_getObjectInShallow(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (input_string[input_string_startIndex] == '{') {

        int i, stack = 1;
        for (i = input_string_startIndex + 1; input_string[i] != '\0'; i++) {

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

// 3-2. Get Array in Shallow
int json_getArrayInShallow(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    if (input_string[input_string_startIndex] == '[') {

        int i, stack = 1;
        for (i = input_string_startIndex + 1; input_string[i] != '\0'; i++) {

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


// 3-3. Get the number with end index
int json_getNumber(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
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

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    int i = input_string_startIndex;
    *output_endIndex = -1; // set default to -1

    // check minus sign
    if (input_string[i] == '-') {
        i++;
    }

integer_part:
    // check integer part first digit
    if (input_string[i] == '0') {
        i++;
        goto fractional_part;
    }

    if (!isdigit(input_string[i])) {
        if (DEBUG) {
            printf("%s: the character at %d (%c 0x%02x) should be a digit\n", __func__, i, input_string[i], input_string[i]);
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
        printf("input_string_startIndex = %d\n", input_string_startIndex);
        puts("================================================================================\n");
        return -1;
    }

    while (isdigit(input_string[++i]));

    // success
    *output_endIndex = i - 1;
    return 0;
}


// 3-4. Get the string with end index
int json_getString(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
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

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    int i = input_string_startIndex;
    *output_endIndex = -1; // set default to -1

    // check first character
    if (input_string[i] != '\"') {
        if (DEBUG) {
            printf("%s: the first character (%c 0x%02x) should be quotation mark (\")\n", __func__, input_string[i], input_string[i]);
        }
        return -1;
    }
    i++;

get_string_loop:

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

    goto get_string_loop;
}

// 3-5. Get the boolean with end index.
int json_getBoolean(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
        return -1;
    }

    if (output_endIndex == NULL) {
        printf("%s: output_endIndex should not be NULL\n", __func__);
        return -1;
    }

    *output_endIndex = -1; // set default to -1

    // check the first character
    const char * booleanValue = input_string[input_string_startIndex] == 't' ? "true" : (input_string[input_string_startIndex] == 'f' ? "false" : NULL);
    if (booleanValue == NULL) {
        return -1;
    }

    // string compare
    int i;
    for (i = 0; booleanValue[i] != '\0'; i++) {
        if (booleanValue[i] != input_string[input_string_startIndex + i]) {
            return -1;
        }
    }

    // success
    *output_endIndex = input_string_startIndex + i - 1;
    return 0;
}

// 3-6. Get the null with end index.
int json_getNull(const char * input_string, const int input_string_startIndex, int * output_endIndex) {
    // check input arguments
    if (input_string == NULL) {
        printf("%s: input_string should not be NULL\n", __func__);
        return -1;
    }

    if (input_string_startIndex < 0) {
        printf("%s: input_string_startIndex (%d) should not be negative\n", __func__, input_string_startIndex);
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
        if (nullValue[i] != input_string[input_string_startIndex + i]) {
            return -1;
        }
    }

    // success
    *output_endIndex = input_string_startIndex + i - 1;
    return 0;
}


// 4-1. get next character without blank
int json_util_getNextCharacter(const char * string, int * index) {
    // check arguments
    if (string == NULL) {
        printf("%s: string should not be NULL\n", __func__);
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

    while (!isprint(string[*index]) || isspace(string[*index])) {
        if (string[*index] == '\0') {
            // it's the end of the string
            return -1;
        }
        (*index)++;
    }

    return 0;
}

// 4-2. print substring
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

// 4-3. allocate substring
int json_util_allocSubstring(const char * string, const int startIndex, const int endIndex, char ** substring) {
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

    if (substring == NULL) {
        printf("%s: substring should not be NULL\n", __func__);
        return -1;
    }

    // set default to NULL
    *substring = NULL;

    char * s = (char *) calloc(endIndex - startIndex + 2, sizeof(char));
    if (s == NULL) {
        printf("%s: out of memory\n", __func__);
        return -1;
    }

    int i;
    for (i = 0; i <= endIndex - startIndex; i++) {
        s[i] = string[startIndex + i];
    }

    *substring = s;
    return 0;
}

// 4-4. string compare
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

// 4-5. allocate string by integer
int json_util_allocStringByInteger(const int number, char ** string) {

    if (number < 0) {
        printf("%s: number (%d) should not be negative\n", __func__, number);
        return -1;
    }

    int digit = 1;
    int n = number;
    while (n / 10 != 0) {
        n /= 10;
        digit++;
    }

    *string = (char *) calloc(digit + 1, sizeof(char));
    if (*string == NULL) {
        printf("%s: out of memory\n", __func__);
        return -1;
    }

    sprintf(*string, "%d", number);

    return 0;
}
