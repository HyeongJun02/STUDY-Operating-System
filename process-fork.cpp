#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MULVALUE 7

bool isClockGettime;

// process가 i*MULVALUE 연산을 하기 위한 함수
void process_task(int start, int end, int process_num) {
    // printf("Process %d - Start: %d, End: %d\n", process_num, start, end);

    // 연산
    for (int i = start; i <= end; ++i) {
        printf("%d * %d = %d\n", i, MULVALUE, i * MULVALUE);
    }

    // 아래 printf는 시간 측정 시 주석 처리하였습니다.
    printf("Process PID:%d\n", (int)getppid()); // Process PID Check
    printf("Process %d end\n", process_num);
    // printf("---------------------------------------------\n");
}

int main() {
	int process_count;
    printf("-----------------------------------------\n");
	printf("Process Count: "); scanf("%d", &process_count);
    printf("-----------------------------------------\n");
    int nRange;
    printf("Range (1~n): "); scanf("%d", &nRange);
    printf("-----------------------------------------\n");
	int repeat;
	printf("Repeat: "); scanf("%d", &repeat);
    printf("-----------------------------------------\n");
	int tf;
	printf("isClockGettime?\n");
	printf("- Use 'clock()': 0\n");
	printf("- Use 'clock_gettime()': 1\n");
	printf("input: "); scanf("%d", &tf);
    printf("-----------------------------------------\n");
	isClockGettime = tf;

    // 반복 후 평균을 내기 위해 REPEAT회 결과값을 더하고 저장할 변수
    double totalSum = 0;
    // 반복 횟수
    // REPEAT만큼 반복
    for(int testCase = 0; testCase < repeat; testCase++) {
        // startTime: 시작 시간
        // endTime: 끝 시간
        struct timespec startGetTime, endGetTime;
        clock_t startTime, endTime;
        if(isClockGettime) {
            // struct timespec startTime, endTime;
            clock_gettime(CLOCK_MONOTONIC, &startGetTime);
        }
        else {
            // clock_t startTime, endTime;
            startTime = clock();
        }
        // totalTime: startTime과 endTime을 연산 후 나온 결과 (실행 시간)
        double totalTime;

        // pid: 프로세스 ID 저장
        pid_t pid;
        // start: 연산할 값의 시작 값
        // end: 연산할 값의 마지막 값 (+ 1)
        // range: 각 프로세스의 처리 범위
        int start, end, range, i;

        range = nRange / process_count;

        // 프로세스의 개수만큼 반복
        for (i = 0; i < process_count; ++i) {
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
        for (i = 0; i < process_count; ++i) {
            wait(NULL);
        }

        if(isClockGettime) {
            clock_gettime(CLOCK_MONOTONIC, &endGetTime);
            totalTime = (endGetTime.tv_sec - startGetTime.tv_sec) + (endGetTime.tv_nsec - startGetTime.tv_nsec) / 1e9;
        }
        else {
            endTime = clock();
            totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        }

        printf("Total Time: %f seconds\n", totalTime);
        totalSum += totalTime;
    }

    // 평균 시간 계산 후 출력
    printf("=====================================================\n");
    printf("[Average time]\n");
    printf("- Process Count: %d\n", process_count);
    printf("- Range: 1~%d\n", nRange);
    printf("- Repeat: %d\n", repeat);
    printf("- Use Function: %s\n", ((isClockGettime==1) ? "clock_gettime()" : "clock()"));
    printf("=====================================================\n");
    printf("=> %f seconds\n", totalSum / repeat);
    printf("=====================================================\n");

    return 0;
}
