#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE_ARRAY_LENGTH 2

void runCat(int pipeEnds[2], int argc, char* argv[]);
void runCut(int pipeEnds[2]);

void exitGraceful(int pipeEnds[2], int errorCode);

int main(int argc, char* argv[]) {
    int pipeEnds[2];
    int pipeRes = pipe(pipeEnds);
    if (pipeRes == -1) {
        perror("pipe");
        exit(1);
    }

    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exitGraceful(pipeEnds, 1);
    }

    if (pid == 0) {
        runCat(pipeEnds, argc, argv);
        exit(1);
    }

    // Parent process (because child must have exited or given control to cat)
    runCut(pipeEnds);
    exit(1);
}

void runCat(int pipeEnds[2], int argc, char* argv[]) {
    int dup2Res = dup2(pipeEnds[1], STDOUT_FILENO);
        if (dup2Res == -1) {
            perror("dup2");
            exitGraceful(pipeEnds, 1);
        }

        close(pipeEnds[0]);
        close(pipeEnds[1]);

        int catArgLen = argc + 1;
        char* catArgs[catArgLen];

        catArgs[0] = "cat";
        catArgs[catArgLen - 1] = NULL;
        for (int i = 1; i < argc; i++) {
            catArgs[i] = argv[i];
        }

        int execRes = execv("/bin/cat", catArgs);
        if (execRes == -1) {
            perror("exec");
        }
}

void runCut(int pipeEnds[2]) {
    int dup2Res = dup2(pipeEnds[0], STDIN_FILENO);
    if (dup2Res == -1) {
        perror("dup2");
        exitGraceful(pipeEnds, 1);
    }

    close(pipeEnds[0]);
    close(pipeEnds[1]);

    char* cutArgs[] = {"cut", "-c", "22-", NULL};
    
    int execRes = execv("/usr/bin/cut", cutArgs);
    if (execRes == -1) {
        perror("exec");
    }
}

void exitGraceful(int pipeEnds[2], int errorCode) {
    for (int i = 0; i < PIPE_ARRAY_LENGTH; i++) {
        close(pipeEnds[i]);
    }

    exit(errorCode);
}
