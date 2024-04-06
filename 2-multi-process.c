#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
<<<<<<< HEAD
#include <sys/wait.h>

#include <pthread.h>
#include <unistd.h>

#include <time.h>

#define RANGE 100
#define THREADCOUNT 4
#define PROCESSCOUNT 4
#define REPEAT 10

// int testCnt;

void* func(void* args) {
    int* params = (int*)args;
    int mul_value = params[0];
    int start = params[1];
    int end = params[2];
    int process_num = params[3];
    int thread_num = params[4];

    // // TEST
    // printf("\n");
    // printf("mul_value: %d\n", mul_value);
    // printf("start: %d\n", start);
    // printf("end: %d\n", end);
    // printf("process_num: %d\n", process_num);
    // printf("thread_num: %d\n", thread_num);
    // printf("\n");

    printf("[Process[%d]-thread[%d] start]\n", process_num, thread_num);

    for (int i = start; i <= end; i++) {
        printf("%3d * %3d = %3d\n", i, mul_value, i * mul_value);
    }

    printf("[Process[%d]-thread[%d] end]\n", process_num, thread_num);

    // args 배열 해제
    free(args);

    pthread_exit(NULL);
}

// process task
void process_task(int process_num) {
    pthread_t tid[THREADCOUNT];

    printf("[Process %d start]\n", process_num);

    /// thread 생성
    for (int i = 0; i < THREADCOUNT; i++) {
        // 각 스레드마다 args 배열을 동적으로 할당 + 초기화
        int* args = (int *)malloc(5 * sizeof(int));
        if (args == NULL) {
            fprintf(stderr, "Memory allocation failed!!\n");
            exit(1);
        }
        // int args[5];
        args[0] = process_num * 2 + 1; // 곱할 값 [3, 5, 7, 9]
        args[2] = (i + 1) * RANGE / THREADCOUNT; // 종료 값 [25, 50, 75, 100]
        args[1] = args[2] - RANGE / THREADCOUNT + 1; // 시작 값 [1, 26, 51, 76]
        args[3] = process_num; // process num
        args[4] = i + 1; // thread num

        if (pthread_create(&tid[i], NULL, func, args) != 0) {
            fprintf(stderr, "thread create error\n");
            exit(1);
        }
        // TEST
        // testCnt++;
        // printf("========= i: %d, mul: %d, process num: %d, thread num: %d, start: %d, end: %d]\n", i, args[0], args[3], args[4], args[1], args[2]);
    }

    for (int i = 0; i < THREADCOUNT; i++) {
        pthread_join(tid[i], NULL);
    }

=======
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void *func(int mul_value, int start, int end) {
    printf("[thread start]\n");
    for(int i=start; i<=end; i++) {
        printf("%d * %d = %d\n", i, mul_value, i*mul_value);
    }
    pthread_exit(NULL);
    printf("[thread end]\n");
}

// process task
void process_task(int range, int process_num) {
    pthread_t tid[4];
    printf("[Process %d start]\n", process_num);
    // thread 생성
    for (int i = 0; i <= 4; ++i) {
        int args[3];
        args[0] = 7; // 곱할 값 설정
        args[1] = process_num * range + 1; // 시작 값 설정
        args[2] = (process_num + 1) * range; // 종료 값 설정
        if (pthread_create(&tid[i], NULL, func, NULL) != 0) {
            fprintf(stderr, "thread create error\n");
            exit(1);
        }
        pthread_join(tid[i], NULL);
    }
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
    printf("[Process %d end]\n", process_num);
}

int main() {
<<<<<<< HEAD
    // 반복 후 평균을 내기 위해 REPEAT회 결과값을 더하고 저장할 변수
    double totalSum = 0;
    // 반복 횟수
    // REPEAT만큼 반복
    for (int testCase = 0; testCase < REPEAT; testCase++) {
        // testCnt = 0;
=======
    // 반복 후 평균을 내기 위해 repeat회 결과값을 더하고 저장할 변수
    double totalSum = 0;
    // 반복 횟수
    int repeat = 10;
    // repeat만큼 반복
    for(int testCase = 0; testCase < repeat; testCase++) {
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
        // startTime: 시작 시간
        // endTime: 끝 시간
        clock_t startTime, endTime;
        // totalTime: startTime과 endTime을 연산 후 나온 결과 (실행 시간)
        double totalTime;

        startTime = clock(); // 시작 시간 기록

        // pid: 프로세스 ID 저장
        pid_t pid;

<<<<<<< HEAD
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
=======
        // start: 연산할 값의 시작 값
        // end: 연산할 값의 마지막 값 (+ 1)
        // range: 각 프로세스의 처리 범위
        int start, end, range, i;

        range = 1000 / 8;

        // 프로세스의 개수만큼 반복
        for (i = 0; i < 8; ++i) {
            start = i * range + 1;
            end = (i + 1) * range;
            
            // 자식 프로세스 생성
            pid = fork();

            if (pid < 0) { // fork 실패
                perror("fork");
                exit(1);
            } else if (pid == 0) { // 자식 프로세스
                // process_task 함수로 이동
                // process_task(시작 값, 마지막 값, 프로세스 번호)
                process_task(start, end, i);
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
                exit(0);
            }
        }

        // 자식 프로세스 종료 대기
<<<<<<< HEAD
        for (int i = 0; i < PROCESSCOUNT; ++i) {
=======
        for (i = 0; i < 8; ++i) {
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
            wait(NULL);
        }

        endTime = clock(); // 종료 시간
        totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC; // 경과 시간

        printf("Total Time: %f seconds\n", totalTime);
        totalSum += totalTime;
    }

    // 평균 시간 계산 후 출력
<<<<<<< HEAD
    printf("[========= mul-pro-th.c END =========]\n");
    printf("[Average time after %d repetitions: %f seconds\n", REPEAT, totalSum / REPEAT);
    // printf("testCnt: %d\n", testCnt);

    return 0;
}
=======
    printf("[Average time after %d repetitions: %f]\n", repeat, totalSum / repeat);

    return 0;
}
>>>>>>> 839fa28b8c449961e36845b612a76fc6505d2c87
