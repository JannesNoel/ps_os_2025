#include <signal.h>
#include <time.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile sig_atomic_t sig_code = -1;

void sigHandler(int sig);
void regHandler(int sig, void (*handler)(int));

int main(void) {
    //for each signal, use sigaction to register signal handler (SIGKILL and SIGSTOP can't be caught, so you could remove them)
    int sigCodes[] = {SIGINT, SIGSTOP, SIGCONT, SIGKILL, SIGUSR1, SIGUSR2};
    size_t arrLen = sizeof(sigCodes) / sizeof(*sigCodes);

    for (size_t i = 0; i < arrLen; i++) {
        int curSig = sigCodes[i];

        regHandler(curSig, sigHandler);
    }


    const time_t work_seconds = 1;

    struct timespec work_duration = {
        .tv_sec = work_seconds,
    };

    struct timespec remaining = {0};

    while (true) {
        // simulate real workload
        if (nanosleep(&work_duration, &remaining) == -1 && errno == EINTR) {
            work_duration = remaining;
            continue;
        }

        // restore work_duration
        work_duration.tv_sec = work_seconds;

        //Handle the recieved signal in a safe manner
        switch(sig_code) {
            case SIGINT:
                printf("SIGINT recieved\n");
                regHandler(SIGUSR1, sigHandler);
                regHandler(SIGUSR2, sigHandler);
                break;
            case SIGSTOP:
                printf("SIGSTOP recieved\n");
                break;
            case SIGCONT:
                printf("SIGCONT recieved\n");
                break;
            case SIGKILL:
                printf("SIGKILL recieved\n");
                break;
            case SIGUSR1:
                printf("SIGUSR1 recieved\n");
                regHandler(SIGUSR2, SIG_IGN);
                break;
            case SIGUSR2:
                printf("SIGUSR2 recieved\n");
                regHandler(SIGUSR1, SIG_IGN);
                break; 
        }

        //Unset the recieved signal to not always print something
        sig_code = -1;
    }

    return EXIT_SUCCESS;
}

void sigHandler(int sig) {
    sig_code = sig;
}

void regHandler(int sig, void (*handler)(int)) {
    struct sigaction actionInfo;
    actionInfo.sa_handler = handler;
    sigemptyset(&actionInfo.sa_mask);
    actionInfo.sa_flags = 0;

    sigaction(sig, &actionInfo, NULL);
}