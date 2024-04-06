#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pthread.h>
#include <unistd.h>

#include <time.h>

#define RANGE 100
#define THREADCOUNT 4
#define PROCESSCOUNT 4

void* func(void* args) {
    int mul_value = *((int*)args);
    int start = *((int*)args + 1);
    int end = *((int*)args + 2);

    printf("[thread start]\n");

    for (int i = start; i <= end; i++) {
        printf("%3d * %3d = %3d\n", i, mul_value, i * mul_value);
    }

    printf("[thread end]\n");

    pthread_exit(NULL);
}

// process task
void process_task(int process_num) {
    pthread_t tid[THREADCOUNT];

    printf("[Process %d start]\n", process_num);

    // thread 생성
    for (int i = 1; i <= THREADCOUNT; i++) {
        int args[3];
        args[0] = process_num * 2 + 1; // 곱할 값 [3, 5, 7, 9]
        args[2] = i * RANGE / THREADCOUNT; // 종료 값 [25, 50, 75, 100]
        args[1] = args[2] - RANGE / THREADCOUNT + 1; // 시작 값 [1, 26, 51, 76]

        if (pthread_create(&tid[i], NULL, func, args) != 0) {
            fprintf(stderr, "thread create error\n");
            exit(1);
        }
        pthread_join(tid[i], NULL);
    }

    printf("[Process %d end]\n", process_num);
}

int main() {
    // 반복 후 평균을 내기 위해 repeat회 결과값을 더하고 저장할 변수
    double totalSum = 0;
    // 반복 횟수
    int repeat = 10;
    // repeat만큼 반복
    for (int testCase = 0; testCase < repeat; testCase++) {
        // startTime: 시작 시간
        // endTime: 끝 시간
        clock_t startTime, endTime;
        // totalTime: startTime과 endTime을 연산 후 나온 결과 (실행 시간)
        double totalTime;

        startTime = clock(); // 시작 시간 기록

        // pid: 프로세스 ID 저장
        pid_t pid;

        // 프로세스의 개수만큼 반복
        for (int i = 1; i <= PROCESSCOUNT; i++) {
            // 자식 프로세스 생성
            pid = fork();
            if (pid < 0) { // fork 실패
                perror("fork");
                exit(1);
            }
            else if (pid == 0) { // 자식 프로세스
                // process_task 함수로 이동
                // process_task(시작 값, 마지막 값, 프로세스 번호)
                process_task(i);
                exit(0);
            }
        }

        // 자식 프로세스 종료 대기
        for (int i = 0; i < PROCESSCOUNT; ++i) {
            wait(NULL);
        }

        endTime = clock(); // 종료 시간
        totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC; // 경과 시간

        printf("Total Time: %f seconds\n", totalTime);
        totalSum += totalTime;
    }

    // 평균 시간 계산 후 출력
    printf("[Average time after %d repetitions: %f]\n", repeat, totalSum / repeat);

    return 0;
}
