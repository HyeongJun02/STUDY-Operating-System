#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define RANGE 100 // 숫자 범위

int turn = 0;
int n[101];
int cnt = 0;
int* flag;

int thread_count;

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

// 1. Dekker
void dekker_critical_section_enter(int thread_id);
void dekker_critical_section_exit(int thread_id);
void* dekker_func(void* args);

// 2. Peterson
void peterson_critical_section_enter(int thread_id);
void peterson_critical_section_exit(int thread_id);
void* peterson_func(void* args);

// 3. Dijkstra
void dijkstra_critical_section_enter(int thread_id);
void dijkstra_critical_section_exit(int thread_id);
void* dijkstra_func(void* args);
enum state { IDLE = 0, WANT_IN, IN_CS };

// 4. Semaphore
void* semaphore_func(void* args);
sem_t sem;

char algorithms[5][30] = { "exit", "Dekker's Algorithm", "Peterson's Algorithm", "Dijkstra's Algorithm", "Semaphore" };

int main() {
    for (int i = 0; i< 101; i++) {
        n[i] = i;
    }

    while(1) {
        int input;

        cnt = 0;
        turn = 0;

        printf("<Select Your Algorithm>\n");
        printf("---------- Select ----------\n");
        for(int i = 0; i < 5; i++) {
            printf("%2d. %s\n", i, algorithms[i]);
        }
        printf("\n");
        printf("input: ");
        if (scanf("%d", &input) != 1) {
            while (getchar() != '\n'); // 입력 스트림을 비움
            system("clear");
            printf("Invalid input. (input type: int)\n\n");
            continue; // 다시 입력 받기
        }

        if (input == 0) break;
        else if (input == 1 || input == 2 || input == 3 || input == 4) {
            printf("================ %s Start ================\n", algorithms[input]);
        }
        else {
            system("clear");
            printf("Wrong input value\n\n");
            continue;
        }

        pthread_t* threads;
        args_t* thread_args;

        int thread_args_count;

        if (input == 1 || input == 2) {
            thread_count = 2;
        }
        else if (input == 3 || input == 4) {
            thread_count = 4;
        }

        // semaphore first setting
        if (input == 4) {
            sem_init(&sem, 0, 1);
        }

        // thread malloc
        threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
        thread_args = (args_t*)malloc(sizeof(args_t) * thread_count);

        // flag malloc
        flag = (int*)malloc(sizeof(int) * thread_count);
        // flag setting
        if (input == 1 || input == 2 || input == 3) {
            for (int i = 0; i < thread_count; i++) {
                flag[i] = 0;
            }
        }
        else if (input == 3) {
            for (int i = 0; i < thread_count; i++) {
                flag[i] = IDLE;
            }
        }

        int step = RANGE / thread_count;

        double aveTime = 0;
        for (int repeat = 0; repeat < 10; repeat++) {
            // TIME!!
            clock_t startTime, endTime;
            double totalTime;
            startTime = clock();

            // thread setting
            for (int i = 0; i < thread_count; i++) {
                thread_args[i].start = i * step + 1;
                thread_args[i].end = (i + 1) * step;
                thread_args[i].thread_id = i;

                // thread create
                if (input == 1) pthread_create(&threads[i], NULL, dekker_func, &thread_args[i]);
                else if (input == 2) pthread_create(&threads[i], NULL, peterson_func, &thread_args[i]);
                else if (input == 3) pthread_create(&threads[i], NULL, dijkstra_func, &thread_args[i]);
                else if (input == 4) pthread_create(&threads[i], NULL, semaphore_func, &thread_args[i]);
            }

            /* function */
            
            // wait to the end
            for (int i = 0; i < thread_count; i++) {
                pthread_join(threads[i], NULL);
            }
            
            // destroy semaphore
            if (input == 4) {
                sem_destroy(&sem);
            }

            // TIME!!
            endTime = clock();
            totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

            printf("\n");
            printf("================ Result ================\n");
            printf("Algorithm           | %s\n", algorithms[input]);
            printf("Mutual exclusion    | %s (cnt: %d)\n", ((cnt == 100) ? "succeeded!" : "failed."), cnt);
            printf("Thread count        | %d\n", thread_count);
            printf("Total Time          | %f seconds\n", totalTime);
            printf("================= End =================\n");
            printf("\n");
            aveTime += totalTime;
        }
        printf("[%lf]\n", aveTime/10);

        free(flag);
        free(threads);
        free(thread_args);
        
        // system("pause");
        printf("Press Enter to continue :)");
        getchar(); // Wait to enter 'Enter' :)
        getchar();
        system("clear");
    }

    return 0;
}

/* 1. Dekker */
void dekker_critical_section_enter(int thread_id) {
    // thread: 크리티컬 섹션 들어갈래
    flag[thread_id] = 1;
    // 상대 스레드의 flag가 올려져있다면
    while (flag[1 - thread_id]) {
        // 내 차례도 아니라면
        if (turn != thread_id) {
            // 대기..
            flag[thread_id] = 0;
            while (turn != thread_id) {}
            // 다시 들어가기
            flag[thread_id] = 1;
        }
    }
}

void dekker_critical_section_exit(int thread_id) {
    // 내 차례 끝났을 경우 turn 양도
    turn = 1 - thread_id;
    // 이제 안 들어갈거야.
    flag[thread_id] = 0;
}

void* dekker_func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        // 데커 알고리즘 크리티컬 영역 입장
        dekker_critical_section_enter(thread_id);
        printf("Thread[%2d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        // 데커 알고리즘 크리티컬 영역 퇴장
        dekker_critical_section_exit(thread_id);
    }
    printf("=== thread[%2d] algorithm end ===\n", thread_id);

    return NULL;
}

/* 2. Peterson */
void peterson_critical_section_enter(int thread_id) {
    // thread 크리티컬 섹션 진입 시도
    flag[thread_id] = 1;
    // 현재 thread가 진입을 시도하므로, 상대 thread의 차례로 turn을 설정
    // 너가 먼저 들어가
    turn = 1 - thread_id;
    // 상대 thread가 진입 시도 & 상대가 크리티컬 섹션에 진입할 차례인 경우 -> 대기
    while (flag[1 - thread_id] && turn == 1 - thread_id) {}
}

void peterson_critical_section_exit(int thread_id) {
    flag[thread_id] = 0;
}

void* peterson_func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        // 피터슨 알고리즘 크리티컬 섹션 진입
        peterson_critical_section_enter(thread_id);
        printf("Thread[%2d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        // 피터슨 알고리즘 크리티컬 섹션 퇴장
        peterson_critical_section_exit(thread_id);
    }
    printf("=== thread[%2d] algorithm end ===\n", thread_id);

    return NULL;
}

/* 3. Dijkstra */
void dijkstra_critical_section_enter(int thread_id) {
    int j;
    do {
        // 임계 영역 진입 시도 1단계
        flag[thread_id] = WANT_IN;
        while (turn != thread_id) {
            if (flag[turn] == IDLE) {
                turn = thread_id;
            }
        }

        // 임계 영역 진입 시도 2단계
        flag[thread_id] = IN_CS;
        j = 0;
        // 자신 이외에 in-CS 영역이 있는지 확인
        while (j < thread_count && (j == thread_id || flag[j] != IN_CS)) {
            j++;
        }
    } while (j < thread_count);
    // 없으면 내가 크리티컬 섹션에 들어가기
}

void dijkstra_critical_section_exit(int thread_id) {
    // flag를 IDLE로 설정
    flag[thread_id] = IDLE;
    // 다음 턴으로
    turn = (turn + 1) % thread_count;
}

void* dijkstra_func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        dijkstra_critical_section_enter(thread_id); // Dijkstra 임계 영역 진입
        printf("Thread[%d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        dijkstra_critical_section_exit(thread_id); // Dijkstra 임계 영역 종료
    }
    printf("=== thread[%2d] algorithm end ===\n", thread_id);

    return NULL;
}

/* 4. Semaphore */
void* semaphore_func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        // 세마포를 통해 크리티컬 섹션 진입 제어
        sem_wait(&sem);
        printf("Thread[%d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        // 크리티컬 섹션 종료 후 세마포 인카운트
        sem_post(&sem);
    }
    printf("=== thread[%2d] algorithm end ===\n", thread_id);

    return NULL;
}