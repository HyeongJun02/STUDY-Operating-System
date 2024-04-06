#include <stdio.h>
#include <time.h>

int main() {
    double totalSum = 0;
    int repeat = 10;
    for(int testCase = 0; testCase < repeat; testCase++) {
        clock_t startTime, endTime;
        double totalTime;

        startTime = clock();
        for (int i = 1; i <= 1000; ++i) {
            printf("%d * %d = %d\n", i, 7, i * 7);
        }

        endTime = clock();
        totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

        printf("Total Time: %f seconds\n", totalTime);
        totalSum += totalTime;
    }

    printf("[Average time after %d repetitions: %f]\n", repeat, totalSum / repeat);

    return 0;
}
