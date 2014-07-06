#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../src/JSON2C.h"

/* Utility Function */
int convertFileToString(const char * input_fileName, char ** output_stringPointer);
int printHexArray(const char * array, int startIndex, int length);

/* Test Function */
void test_valueTypeDescription();
void test_getString();
void test_getNumber();
void test_getBoolean();
void test_getNull();
void test_getValue();
void test_getArrayValueByPosition();

/* Main */
int main() {
    const char * fileName = "sample.json";

    char * string; // need to be free
    if (convertFileToString(fileName, &string) != 0) {
        printf("convert file '%s' to string failure\n", fileName);
        return EXIT_SUCCESS;
    }

    printf("%s (%lu) = %s\n", fileName, strlen(string), string);

    test_valueTypeDescription();
    test_getString();
    test_getNumber();
    test_getBoolean();
    test_getNull();
    test_getValue();
    test_getArrayValueByPosition();

    free(string);
    return EXIT_SUCCESS;
}

int convertFileToString(const char * input_fileName, char ** output_stringPointer) {
    const int DEBUG = 0;

    // 1. check input file
    if (input_fileName == NULL || strlen(input_fileName) == 0) {
        puts("File Name should not be NULL or empty\n");
        return -1;
    }

    // 2. read file
    FILE * fp = fopen(input_fileName, "r");
    if (fp == NULL) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    // 3. calculate file content length
    int len = 0;
    while (fgetc(fp) != EOF) {
        len++;
    }

    // 4. alloc string
    char * str = (char *) calloc(len + 1, sizeof(char));
    if (str == NULL) {
        printf("%s\n", strerror(errno)); // Out of memory
        fclose(fp);
        return -1;
    }

    if (DEBUG) {
        printf("convertFileToString: alloc str byte array (len + 1 = %d)\n", len + 1);
        printHexArray(str, 0, len + 1);
        puts("");
    }

    rewind(fp); // trun to starting point

    // 5. store content to str
    int i;
    for (i = 0; i < len; i++) {
        str[i] = fgetc(fp);
    }
    // str[len] = '\0';  // we have already do memset in calloc, so it doesn't have to set '\0' at the end again

    // close fp
    fclose(fp);

    if (DEBUG) {
        printf("convertFileToString: set str byte array (len + 1 = %d)\n", len + 1);
        printHexArray(str, 0, len + 1);
        puts("");
    }

    // 6. set output
    *output_stringPointer = str;
    return 0;
}

int printHexArray(const char * array, int startIndex, int length) {
    int i;
    for (i = startIndex; i < startIndex + length; i++) {
        printf("0x%02x ", array[i]);

        // wrap line
        if (i % 10 == 9)   puts("");
        if (i % 100 == 99) puts("");
    }
    puts("");

    return 0;
}

void test_valueTypeDescription() {
    puts("Test json type description");
    puts("================================================================================");
    int i;
    for (i = -1; i <= 7; i++) {
        printf("%s\n", json_valueTypeDescription(i));
    }
    puts("================================================================================\n");
}

void test_getString() {
    puts("Test json_getString");
    puts("================================================================================");

    char * str[100] = {
        "\"\"ab c\"",
        "",
        "\"abc\"",
        "abc",
        "\"",
        "\"abc\"abc",
        "\"abc\\",
        "\"abc",
        "\"\"abc\"",                      /*   "abc               */
        "\"\\abc\"",                      /*   \abc               */
        "\"/abc\"",                       /*   /abc               */
        "\"\\/abc\"",                     /*   \/abc              */
        "\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"", /*   \"\\\/\b\f\n\r\t   */
        "\"abc\\u\"",
        "\"abc\\u1\"",
        "\"abc\\u12\"",
        "\"abc\\u123\"",
        "\"abc\\u1234\"",
        "\"abc\\u1234",
        "\"abc\\u1a34\"",
        "\"abc\\u1A3456\"",
        "\"abc\\u12s4\"",
        "\"abc\\u1234\\u1234abc\""
    };

    int i, startIndex, endIndex;
    for (i = 0; str[i] != NULL; i++) {
        startIndex = 0;
        if (json_getString(str[i], startIndex, &endIndex) == 0) {
            printf("%d. %s [%d..%d] is string\n\n", i + 1, str[i], startIndex, endIndex);
        } else {
            printf("%d. %s is not string\n\n", i + 1, str[i]);
        }
    }
    puts("");

    char * s = str[0]; // ""ab c"
    startIndex = 1;
    if (json_getString(s, startIndex, &endIndex) == 0) {
        printf("%s [%d..%d] is string\n\n", s, startIndex, endIndex);
    } else {
        printf("%s [%d..] is not string\n\n", s, startIndex);
    }

    puts("================================================================================\n");
}

void test_getNumber() {
    puts("Test json_getNumber");
    puts("================================================================================");

    char * str[100] = {
        "--123",
        "abc",
        "123\n",
        "456",
        "-",
        "-abc",
        "-123",
        "0",
        "-0",
        "0.",
        ".",
        "0.0",
        "1.2.3",
        "-10.123",
        "10.e",
        "10..",
        "e",
        "-E",
        "-0e12",
        "10.5e00abc",
        "10.5E+13",
        "8.5E-15",
        "8e",
        "123E-",
        "456E+",
        "9.e",
        "9.e99",
        "0ee",
        "",
        "0.0eabc",
        "0.0e-abc"
    };

    int i, startIndex, endIndex;
    for (i = 0; str[i] != NULL; i++) {
        startIndex = 0;
        if (json_getNumber(str[i], startIndex, &endIndex) == 0) {
            printf("%d. %s [%d..%d] is number\n\n", i + 1, str[i], startIndex, endIndex);
        } else {
            printf("%d. %s is not number\n\n", i + 1, str[i]);
        }
    }

    char * s = str[0]; // --123
    startIndex = 1;
    if (json_getNumber(s, startIndex, &endIndex) == 0) {
        printf("%s [%d..%d] is number\n\n", s, startIndex, endIndex);
    } else {
        printf("%s [%d..] is not number\n\n", s, startIndex);
    }

    puts("================================================================================\n");
}

void test_getBoolean() {
    puts("Test json_getBoolean");
    puts("================================================================================");

    char * str[100] = {
        "abcfalseabc",
        "0ee",
        "true",
        "false",
        "tabc",
        "fabc",
        "",
        "t",
        "f"
    };

    int i, startIndex, endIndex;
    for (i = 0; str[i] != NULL; i++) {
        startIndex = 0;
        if (json_getBoolean(str[i], startIndex, &endIndex) == 0) {
            printf("%d. %s [%d..%d] is boolean\n\n", i + 1, str[i], startIndex, endIndex);
        } else {
            printf("%d. %s is not boolean\n\n", i + 1, str[i]);
        }
    }

    char * s = str[0]; // abctrue
    startIndex = 3;
    if (json_getBoolean(s, startIndex, &endIndex) == 0) {
        printf("%s [%d..%d] is boolean\n\n", s, startIndex, endIndex);
    } else {
        printf("%s [%d..] is not boolean\n\n", s, startIndex);
    }

    puts("================================================================================\n");
}

void test_getNull() {
    puts("Test json_getNull");
    puts("================================================================================");

    char * str[100] = {
        "abcnullabc",
        "null",
        "n",
        "nullabc",
        ""
    };

    int i, startIndex, endIndex;
    for (i = 0; str[i] != NULL; i++) {
        startIndex = 0;
        if (json_getNull(str[i], startIndex, &endIndex) == 0) {
            printf("%d. %s [%d..%d] is null\n\n", i + 1, str[i], startIndex, endIndex);
        } else {
            printf("%d. %s is not null\n\n", i + 1, str[i]);
        }
    }

    char * s = str[0]; // abctrue
    startIndex = 3;
    if (json_getNull(s, startIndex, &endIndex) == 0) {
        printf("%s [%d..%d] is null\n\n", s, startIndex, endIndex);
    } else {
        printf("%s [%d..] is not null\n\n", s, startIndex);
    }

    puts("================================================================================\n");
}

void test_getValue() {
    puts("Test json_getValue");
    puts("================================================================================");

    char * str[100] = {
        "abcnullabc",
        "null",
        "n",
        "nullabc",
        "",
        "abcfalseabc",
        "0ee",
        "true",
        "false",
        "tabc",
        "fabc",
        "t",
        "f",
        "--123",
        "abc",
        "123\n",
        "456",
        "-",
        "-abc",
        "-123",
        "0",
        "-0",
        "0.",
        ".",
        "0.0",
        "1.2.3",
        "-10.123",
        "10.e",
        "10..",
        "e",
        "-E",
        "-0e12",
        "10.5e00abc",
        "10.5E+13",
        "8.5E-15",
        "8e",
        "123E-",
        "456E+",
        "9.e",
        "9.e99",
        "0ee",
        "0.0eabc",
        "0.0e-abc",
        "\"\"ab c\"",
        "",
        "\"abc\"",
        "abc",
        "\"",
        "\"abc\"abc",
        "\"abc\\",
        "\"abc",
        "\"\"abc\"",                      /*   "abc               */
        "\"\\abc\"",                      /*   \abc               */
        "\"/abc\"",                       /*   /abc               */
        "\"\\/abc\"",                     /*   \/abc              */
        "\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"", /*   \"\\\/\b\f\n\r\t   */
        "\"abc\\u\"",
        "\"abc\\u1\"",
        "\"abc\\u12\"",
        "\"abc\\u123\"",
        "\"abc\\u1234\"",
        "\"abc\\u1234",
        "\"abc\\u1a34\"",
        "\"abc\\u1A3456\"",
        "\"abc\\u12s4\"",
        "\"abc\\u1234\\u1234abc\"",
        "{}",
        "{1234567890}",
        "{12345{12345{12345",
        "{12345{12345{12345}}}",
        "{12345{12345}}}}",
        "{12345{{{12345}}}",
        "[]",
        "[1234567890]",
        "[12345[12345[12345",
        "[12345[12345[12345]]]",
        "[12345[12345]]]]",
        "[12345[[[12345]]]"
    };

    int i, startIndex, endIndex, jsonType;
    for (i = 0; str[i] != NULL; i++) {
        startIndex = 0;
        if (json_getValue(str[i], startIndex, &endIndex, &jsonType) == 0) {
            printf("%d. %s [%d..%d] is %s\n\n", i + 1, str[i], startIndex, endIndex, json_valueTypeDescription(jsonType));
        } else {
            printf("%d. %s is not JSON value\n\n", i + 1, str[i]);
        }
    }

    puts("================================================================================\n");
}

void test_getArrayValueByPosition() {
    puts("Test json_getArrayValueByPosition");
    puts("================================================================================");

    char * str =
    "abc[\n"
    "    \"Hello\\\"\",		\n"
    "    { object }    ,\n"
    "    [ array ],\n"
    "    [12345[12345[12345]]]\n,"
    "    \"\\\"\\\\\\/\\b\\f\\n\\r\\t\",\n"
    "    10.5E+13,\n"
    "    true,\n"
    "    false, \n"
    "    null\n"
    "]]]]";

    printf("Array (%lu) = %s\n\n", strlen(str), str);

    int i, valueStartIndex, valueEndIndex, valueJsonType;
    for (i = 0; i < 10; i++) {

        if (json_getArrayValueByPosition(str, 3, i, &valueStartIndex, &valueEndIndex, &valueJsonType) == -1) {
            printf("Array[%d] is not found\n\n", i);
            continue;
        }

        // value is found
        printf("Array[%d] = ", i);
        int j;
        for (j = valueStartIndex; j <= valueEndIndex; j++) {
            printf("%c", str[j]);
        }
        printf(" (%s)\n\n", json_valueTypeDescription(valueJsonType));
    }

    puts("================================================================================\n");
}
