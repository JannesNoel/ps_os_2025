#include <pthread.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

void printThreadCreateError(int errCode);
void printThreadJoinError(int errCode);

void* sumFile(void* fileHandle) {
    int* result = calloc(1, sizeof(*result));
    FILE* file = fopen((char*) fileHandle, "r");

    int curVal;
    while (fscanf(file, "%d\n", &curVal) == 1) {
        *result += curVal;
    }

    fclose(file);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s <file_paths>...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int threadCount = argc - 1;
    pthread_t threadIds[threadCount];
    int* threadResults[threadCount];

    for (int i = 0; i < threadCount; i++) {
        int threadCreationCode = pthread_create(&threadIds[i], NULL, sumFile, argv[i + 1]);
        if (threadCreationCode != 0) {
            printThreadCreateError(threadCreationCode);
        }
    }

    for (int i = 0; i < threadCount; i++) {
        int threadJoinCode = pthread_join(threadIds[i], (void**) &threadResults[i]);
        if (threadJoinCode != 0) {
            printThreadJoinError(threadJoinCode);
        }
    }

    int sum = 0;
    for (int i = 0; i < threadCount; i++) {
        printf("sum %d = %d\n", i + 1, *threadResults[i]);
        sum += *threadResults[i];

        free(threadResults[i]);
    }
    printf("total sum = %d\n", sum);

    return EXIT_SUCCESS;
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

