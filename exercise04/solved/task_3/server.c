#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_BASE_NAME "/tmp/fifo_csbb9456_"
#define FIFO_BASE_NAME_LEN strlen(FIFO_BASE_NAME)

#ifndef PIPE_BUF
    #define PIPE_BUF 512 
#endif

void makeFifoPaths(int argc, char* argv[], char** fifoPaths);
void waitForConnection(char** fifoPaths, int* openFifoFds, int numClients);
void pollMessages(char** fifoPaths, int* openFifoFds, int numClients);

void disconnectClient(bool* connected, char* clientName);
void prepareExit(char** fifoPaths, int* openFifoFds, int numClients);
int getOpenConnectionsNum(bool* openConnections, int numClients);
char* getNameFromPath(char*);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("USAGE: %s <client_name1> [client_names...]\n", argv[0]);
        exit(1);
    }

    int numClients = argc - 1;
    char** fifoPaths = malloc(numClients * sizeof(*fifoPaths));
    makeFifoPaths(argc, argv, fifoPaths);

    int* openFifoFds = malloc(numClients * sizeof(*openFifoFds));
    waitForConnection(fifoPaths, openFifoFds, numClients);

    pollMessages(fifoPaths, openFifoFds, numClients);
    prepareExit(fifoPaths, openFifoFds, numClients);
    exit(0);
}

void makeFifoPaths(int argc, char* argv[], char** fifoPaths) {
    for (int i = 1; i < argc; i++) {
        int fifoPathsIndex = i - 1;
        // Construct the fifo path
        // Calculate length with nullterminator
        int fifoNameLen = FIFO_BASE_NAME_LEN + strlen(argv[i]) + 1;
        fifoPaths[fifoPathsIndex] = malloc(fifoNameLen * sizeof(**fifoPaths));

        // Add the base name 
        for (unsigned long j = 0; j < FIFO_BASE_NAME_LEN; j++) {
            fifoPaths[fifoPathsIndex][j] = FIFO_BASE_NAME[j];
        }

        // Add the client name
        for (unsigned long j = 0; j < strlen(argv[i]); j++) {
            fifoPaths[fifoPathsIndex][FIFO_BASE_NAME_LEN + j] = argv[i][j];
        }

        // Finally, add the nullterminator
        fifoPaths[fifoPathsIndex][fifoNameLen - 1] = '\0';

        // Set fifo permissions
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        // Make the fifo
        int fifoRet = mkfifo(fifoPaths[fifoPathsIndex], mode);
        if (fifoRet == -1) {
            perror("fifo");
            prepareExit(fifoPaths, NULL, argc - 1);
            exit(1);
        }
    }
}

void waitForConnection(char** fifoPaths, int* openFifoFds, int numClients) {
    for (int i = 0; i < numClients; i++) {
        openFifoFds[i] = open(fifoPaths[i], O_RDONLY);
        if (openFifoFds[i] < 0) {
            perror("openFifo");
            prepareExit(fifoPaths, openFifoFds, numClients);
            exit(1);
        }

        printf("%s connected.\n", getNameFromPath(fifoPaths[i]));
    }
}

void pollMessages(char** fifoPaths, int* openFifoFds, int numClients) {
    struct pollfd pollFds[numClients];
    bool connectedClients[numClients];

    // create the pollfd structs
    for (int i = 0; i < numClients; i++) {
        pollFds[i].fd = openFifoFds[i];
        pollFds[i].events = POLLIN;

        connectedClients[i] = true;
    }

    // init the vars needed for the poll-loop
    unsigned messageCounter = 0;
    char messageBuf[PIPE_BUF + 1];

    // poll for new messages, as long as at least one connection is open
    for (
        int openConnections = numClients; 
        openConnections > 0; 
        openConnections = getOpenConnectionsNum(connectedClients, numClients)
    ) {
        int pollRes = poll(pollFds, numClients, -1);
        if (pollRes == -1) {
            perror("poll");
            return;
        }

        // loop over all clients to see if they have sent a new message or not
        for (int i = 0; i < numClients; i++) {
            // if the client is not connected anymore, don't check them
            if (!connectedClients[i]) continue;
    
            // if the POLLIN Flag is set in the revents, then a new message was recieved
            if (pollFds[i].revents & POLLIN) {
                char* clientName = getNameFromPath(fifoPaths[i]);
                ssize_t readBytes = read(openFifoFds[i], messageBuf, PIPE_BUF);
            
                if (readBytes > 0) {

                    // if the message is empty, disconnect the client (bc the bug with poll also appears to happen on the zid server),
                    // else increase the message counter and print the message
                    if (messageBuf[0] == '\0') {
                        disconnectClient(&connectedClients[i], clientName);
                    } else {
                        messageCounter++;
                        printf("Message %d: \"%s\" from %s\n", messageCounter, messageBuf, clientName);
                    }
                } else if (readBytes == 0) {
                    // The bug of poll seems to also occur on linux, so this never triggers
                    disconnectClient(&connectedClients[i], clientName);
                } else {
                    perror("read");
                    prepareExit(fifoPaths, openFifoFds, numClients);
                    exit(1);
                }
            }
        }
    }

    printf("All clients disconnected.\n");
}

void disconnectClient(bool* connected, char* clientName) {
    printf("%s disconnected.\n", clientName);
    *connected = false;
}

void prepareExit(char** fifoPaths, int* openFifoFds, int numClients) {
    if (fifoPaths != NULL) {
        for (int i = 0; i < numClients; i++) {
            if (fifoPaths[i] != NULL) {
                unlink(fifoPaths[i]);
                free(fifoPaths[i]);
            }
        }

        free(fifoPaths);
    }

    if (openFifoFds != NULL) {
        for (int i = 0; i < numClients; i++) {
            close(openFifoFds[i]);
        }

        free(openFifoFds);
    }
}

int getOpenConnectionsNum(bool* connectedClients, int numClients) {
    int returnVal = 0;
    
    for (int i = 0; i < numClients; i++) {
        returnVal += connectedClients[i];
    }

    return returnVal;
}

char* getNameFromPath(char* pathStr) {
    return pathStr + FIFO_BASE_NAME_LEN;
}
