#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>

#define NS_PER_SEC 1000000000

double timeDiffInS(struct timespec start, struct timespec end);
double DR_p(const int t, const int n, const unsigned long long s);

int main(int argc, char** argv) {
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    if (argc != 3) {
        fprintf(stderr, "USAGE: \"%s <num_dice_sides> <num_simulations>\"\n\n", argv[0]);
        return 1;
    }

    int diceSides = atoi(argv[1]);
    unsigned long long numSimulations = atol(argv[2]);

    int numProcesses = 2 * diceSides - 1;

    for (int i = 0; i < numProcesses; i++) {
        if (fork() == 0) {
            srand(getpid());
            struct timespec finish;

            int targetNum = i + 2;
            double probability = DR_p(targetNum, diceSides, numSimulations);
            clock_gettime(CLOCK_REALTIME, &finish);

            printf(
                "Child %d PID = %d. DR_p(%d, %d, %llu) = %f. Elapsed time = %fs\n",
                i,
                getpid(),
                targetNum,
                diceSides,
                numSimulations,
                probability,
                timeDiffInS(start, finish)
            );

            exit(0);
        }
    }

    while (wait(NULL) > 0);

    printf("Done\n");
}

double DR_p(const int t, const int n, const unsigned long long s) {
    unsigned long long h = 0;
    for (unsigned long long i = s; i--;) {
      h += (rand() % n + rand() % n + 2 == t);
    }
    return (double)h / s;
}

double timeDiffInS(struct timespec start, struct timespec finish){
    int secondsDiff = finish.tv_sec - start.tv_sec;
    long long nanoSecDiff = finish.tv_nsec - start.tv_nsec;

    if (secondsDiff > 0 && nanoSecDiff < 0) {
        secondsDiff--;
        nanoSecDiff += NS_PER_SEC;
    } else if (secondsDiff < 0 && nanoSecDiff > 0) {
        secondsDiff++;
        nanoSecDiff -= NS_PER_SEC;
    }

    return secondsDiff + ((double) nanoSecDiff / NS_PER_SEC);
}
