#include <stdio.h>
#include <time.h>

#define REPEAT 10

double totalSum = 0;

void task() {
	clock_t startTime, endTime;
	double totalTime;
	startTime = clock();
	for (int i = 1; i <= 4; i++) {
		printf("[%2d]\n", i);
		for (int j = 1; j <= 100; j++) {
<<<<<<< HEAD
			printf("%3d * %3d = %3d\n", j, i * 2 + 1, j * (i * 2 + 1));
=======
			printf("%d * %d = %d\n", j, i * 2 + 1, j * (i * 2 + 1));
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
		}
	}
	endTime = clock();
	totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("Total Time: %f seconds\n", totalTime);
	totalSum += totalTime;
}
int main() {
	for (int testCase = 0; testCase < REPEAT; testCase++) {
		task();
	}
<<<<<<< HEAD
    printf("[========= sin-pro.c END =========]\n");
	printf("[Average time after %d repetitions: %f seconds]\n", REPEAT, totalSum / REPEAT);
=======
	printf("[Average time after %d repetitions: %f]\n", REPEAT, totalSum / REPEAT);
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87

	return 0;
}