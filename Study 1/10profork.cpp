#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// 프로세스의 개수
#define PROCESSCOUNT 10
#define MULVALUE 7

// process가 i*MULVALUE 연산을 하기 위한 함수
void process_task(int start, int end, int process_num) {
    // printf("Process %d - Start: %d, End: %d\n", process_num, start, end);

    // 각 프로세스에서의 시간도 측정해보기 위해 시간 측정 알고리즘 추가
    clock_t startTime = clock();
    // 연산
    for (int i = start; i <= end; ++i) {
        printf("%d * %d = %d\n", i, MULVALUE, i * MULVALUE);
    }
    clock_t endTime = clock();
    double totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    // 아래 printf는 시간 측정 시 주석 처리하였습니다.
    printf("Process PID:%d\n", (int)getppid()); // Process PID Check
    printf("Process %d end\n", process_num);
    // printf("Process %d Elapsed Time: %f seconds\n", process_num, totalTime);
    // printf("---------------------------------------------\n");
}

int main() {
    // 반복 후 평균을 내기 위해 repeat회 결과값을 더하고 저장할 변수
    double totalSum = 0;
    // 반복 횟수
    int repeat = 10;
    // repeat만큼 반복
    for(int testCase = 0; testCase < repeat; testCase++) {
        // startTime: 시작 시간
        // endTime: 끝 시간
        clock_t startTime, endTime;
        // totalTime: startTime과 endTime을 연산 후 나온 결과 (실행 시간)
        double totalTime;

        startTime = clock(); // 시작 시간 기록

        // pid: 프로세스 ID 저장
        pid_t pid;
        // start: 연산할 값의 시작 값
        // end: 연산할 값의 마지막 값 (+ 1)
        // range: 각 프로세스의 처리 범위
        int start, end, range, i;

        range = 1000 / PROCESSCOUNT;

        // 프로세스의 개수만큼 반복
        for (i = 0; i < PROCESSCOUNT; ++i) {
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
                exit(0);
            }
        }

        // 자식 프로세스 종료 대기
        for (i = 0; i < PROCESSCOUNT; ++i) {
            wait(NULL);
        }

        endTime = clock(); // 종료 시간
        totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC; // 경과 시간

        printf("Total Time: %f seconds\n", totalTime);
        totalSum += totalTime;
    }

    // 평균 시간 계산 후 출력
    printf("[Average time after %d repetitions: %f]", repeat, totalSum / repeat);

    return 0;
}
