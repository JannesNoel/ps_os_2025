#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>

#define FIFO_BASE_NAME "/tmp/fifo_csbb9456_"
#define FIFO_BASE_NAME_LEN strlen(FIFO_BASE_NAME)

#ifndef PIPE_BUF
    #define PIPE_BUF 512 
#endif

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("USAGE: %s <name>\n", argv[0]);
        exit(1);
    }


    // Construct the path to the pipe
    char fifoPath[FIFO_BASE_NAME_LEN + strlen(argv[1]) + 1];
    for (unsigned long i = 0; i < FIFO_BASE_NAME_LEN; i++) {
        fifoPath[i] = FIFO_BASE_NAME[i];
    }

    for (unsigned long i = 0; i < strlen(argv[1]); i++) {
        fifoPath[FIFO_BASE_NAME_LEN + i] = argv[1][i];
    }
    fifoPath[FIFO_BASE_NAME_LEN + strlen(argv[1])] = '\0';


    // Open the Fifo
    int fifoFd = open(fifoPath, O_WRONLY);
    if (fifoFd == -1) {
        perror("open");
        exit(1);
    }

    bool connectionOpen = true;
    char fifoBuffer[PIPE_BUF];
    
    while (connectionOpen) {
        printf("\nMessage:\n");
        fgets(fifoBuffer, PIPE_BUF, stdin);
        
        size_t fifoMessageLen = strlen(fifoBuffer);
        fifoBuffer[fifoMessageLen - 1] = '\0';

        if(fifoBuffer[0] == '\0') {
            connectionOpen = false;
            write(fifoFd, fifoBuffer, strlen(fifoBuffer));
            close(fifoFd);
        } 
        
        if (connectionOpen) {
            write(fifoFd, fifoBuffer, strlen(fifoBuffer));
        }
    }

    exit(0);
}
