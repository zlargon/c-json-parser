#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../src/JSON2C.h"

/* Utility Function */
int convertFileToString(const char * input_fileName, char ** output_stringPointer);
int printHexArray(const char * array, int startIndex, int length);

/* Main */
int main() {
    const char * fileName = "sample.json";

    char * string; // need to be free
    if (convertFileToString(fileName, &string) != 0) {
        printf("convert file '%s' to string failure\n", fileName);
        return EXIT_SUCCESS;
    }

    printf("%s (%lu) = %s\n", fileName, strlen(string), string);
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
