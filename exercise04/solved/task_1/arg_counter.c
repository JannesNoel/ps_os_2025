#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ARG_AMT 10

enum EXIT_CODES {
    EXIT_NO_ARGS = 13,
    EXIT_TOO_MANY_ARGS = 7,
    EXIT_ARG1_INVALID = 50,
    EXIT_INVALID_OFFSET = 99
};

bool stringIsNumber(char* str);

int main(int argc, char* argv[]) {
    // Not really necessary, but helps with understandability
    int argCWithoutName = argc - 1;
    
    if (argCWithoutName < 1) {
        fprintf(stderr, "Usage: %s <arg1> [arg2 ... arg10]\n", argv[0]);
        exit(EXIT_NO_ARGS);
    }
    
    if (argCWithoutName > MAX_ARG_AMT) {
        exit(EXIT_TOO_MANY_ARGS);
    }

    char* offsetStr = getenv("OFFSET");
    if (offsetStr == NULL || ! stringIsNumber(offsetStr)) {
        exit(EXIT_INVALID_OFFSET);
    }
    int offset = atoi(offsetStr);

    // Custom constraint (as required by the task): The first argument has to be a Base-10 Int
    if (! stringIsNumber(argv[1])) {
        exit(EXIT_ARG1_INVALID);
    }

    int result = argCWithoutName + offset;
    printf("Result: %d\n", result);

    exit(EXIT_SUCCESS);
}

bool stringIsNumber(char* str) {
    char* endPtr;
    strtol(str, &endPtr, 10);

    if (endPtr == NULL || *endPtr == '\0') {
        return true;
    }

    return false;
}