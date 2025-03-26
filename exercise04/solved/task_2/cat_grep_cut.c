#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE_ARRAY_LENGTH 2

#define READ_END 0
#define WRITE_END 1

void runCat(int pipeEnds[2], int argc, char* argv[]);
void runGrep(int catGrepPipes[2], int grepCutPipes[2]);
void runCut(int pipeEnds[2]);

void pipeHandleErr(int pipeEnds[2]);
void closePipe(int pipeEnds[2]);
void exitGraceful(int pipeEnds[2], int errorCode);

int main(int argc, char* argv[]) {
    int catGrepPipes[2];
    pipeHandleErr(catGrepPipes);

    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exitGraceful(catGrepPipes, 1);
    } else if (pid == 0) {
        runCat(catGrepPipes, argc, argv);
        exit(1);
    }

    // Parent process (because child must have exited or given control to cat)
    int grepCutPipes[2];
    pipeHandleErr(grepCutPipes);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exitGraceful(catGrepPipes, 1);
    } else if (pid == 0) {
        runGrep(catGrepPipes, grepCutPipes);
        exit(1);
    }

    // Back in parent process again 
    closePipe(catGrepPipes);
    runCut(grepCutPipes);
    exit(1);
}

void runCat(int pipeEnds[2], int argc, char* argv[]) {
    int dup2Res = dup2(pipeEnds[WRITE_END], STDOUT_FILENO);
    if (dup2Res == -1) {
        perror("dup2");
        exitGraceful(pipeEnds, 1);
    }
    closePipe(pipeEnds);

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

void runGrep(int catGrepPipes[2], int grepCutPipes[2]) {
    int dup2Res = dup2(catGrepPipes[READ_END], STDIN_FILENO);
    if (dup2Res == -1) {
        perror("dup2");

        closePipe(grepCutPipes);
        exitGraceful(catGrepPipes, 1);
    }
    closePipe(catGrepPipes);

    dup2Res = dup2(grepCutPipes[WRITE_END], STDOUT_FILENO);
    if (dup2Res == -1) {
        perror("dup2");
        exitGraceful(grepCutPipes, 1);
    }
    closePipe(grepCutPipes);

    char* grepArgs[] = {"grep", "^;", NULL};
    int execRes = execv("/usr/bin/grep", grepArgs);
    if (execRes == -1) {
        perror("exec");
    }
}

void runCut(int pipeEnds[2]) {
    int dup2Res = dup2(pipeEnds[READ_END], STDIN_FILENO);
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


void pipeHandleErr(int pipeEnds[2]) {
    int pipeRes = pipe(pipeEnds);
    if (pipeRes == -1) {
        perror("pipe");
        exit(1);
    }
}

void closePipe(int pipeEnds[2]) {
    for (int i = 0; i < PIPE_ARRAY_LENGTH; i++) {
        close(pipeEnds[i]);
    }
}

void exitGraceful(int pipeEnds[2], int errorCode) {
    closePipe(pipeEnds);
    exit(errorCode);
}
