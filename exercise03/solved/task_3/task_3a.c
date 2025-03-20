#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

int64_t accumulation = 0;

void printForkError(int errorCode);
void printThreadCreateError(int errorCode);
void printThreadJoinError(int errorCode);

void* accumulate(void* arg);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    long N = atol(argv[1]);

    printf("Accumulation before fork: %ld\n", accumulation);

    int pid = fork();
    if (pid == -1) {
        printForkError(errno);
        return EXIT_FAILURE;
    } else if (pid == 0) {
        accumulate(&N);
        exit(0);
    }

    while (wait(NULL) > 0);
    printf("Accumulation after fork: %ld\n", accumulation);

    pthread_t threadHandle;
    int threadCreateCode = pthread_create(&threadHandle, NULL, accumulate, &N);
    if (threadCreateCode != 0) {
        printThreadCreateError(threadCreateCode);
        return EXIT_FAILURE;
    }

    int threadJoinCode = pthread_join(threadHandle, NULL);
    if (threadJoinCode != 0) {
        printThreadJoinError(threadJoinCode);
        return EXIT_FAILURE;
    }

    printf("Accumulation after thread: %ld\n", accumulation);
    return EXIT_SUCCESS;

    /*
        Observation: The main thread and the newly created thread share the global variable (heap memory),
        while the child-process gets a copy of the stack and the heap, so no sharing can happen.
    */
}

void* accumulate(void* arg) {
    int N = *((int*) arg);

    for (int i = 1; i <= N; i++) {
        accumulation += i;
    }

    return NULL;
}

void printForkError(int errorCode) {
    switch (errorCode) {
        case EAGAIN:
            fprintf(stderr, "Failed to create a child-process: A system-imposed limit on threads/child-processes stopped the child process from being created\n");
            break;
        case ENOMEM:
            fprintf(stderr, "Failed to create a child-process: Insufficient memory or trying to creating in a pid namespace, which init process had failed\n");
            break;
        case ENOSYS:
            fprintf(stderr, "Failed to create a child-process: The \"fork()\" function is not supported on this platform");
            break;
    }
}

void printThreadCreateError(int errorCode) {
    switch(errorCode) {
        case EAGAIN:
            fprintf(stderr, "Failed to create new thread: Insufficient resources or a system-imposed thread limit is preventing the creation\n");
            break;
        case EINVAL:
            fprintf(stderr, "Failed to create new thread: Invalid settings in the attributes of the thread\n");
            break;
        case  EPERM:
            fprintf(stderr, "Failed to create new thread: Insufficient permissions to use the specified thread attributes\n");
            break;
    }
}

void printThreadJoinError(int errorCode) {
    switch (errorCode) {
        case EDEADLK:
            fprintf(stderr, "Failed to join thread: A deadlock has occurred or the thread specifies the calling thread\n");
            break;
        case EINVAL:
            fprintf(stderr, "Failed to join thread: The thread is not joinable or another thread is already waiting to join with this thread\n");
            break;
        case ESRCH:
            fprintf(stderr, "Failed to join thread: The specified thread_id could not be found!\n");
            break;
    }
}
