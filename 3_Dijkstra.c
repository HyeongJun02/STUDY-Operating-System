#include <stdio.h>
#include <pthread.h>

#define N 4 // 스레드 수
#define RANGE 100 // 범위

int flag[N] = {0}; // N개의 스레드의 플래그 배열
int turn = 0; // 턴을 나타내는 변수
int n[101]; // 1부터 100까지의 숫자를 저장하는 배열
int cnt = 0; // 출력된 수의 카운트

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

// Dijkstra's algorithm: enter_critical_section
void enter_critical_section(int thread_id) {
    for (int i = 0; i < N; i++) {
        flag[thread_id] = i; // 스레드의 플래그를 i로 설정
        turn = thread_id; // 턴을 현재 스레드로 설정

        int other_thread_id;
        for (other_thread_id = 0; other_thread_id < N; other_thread_id++) {
            if (other_thread_id != thread_id && flag[other_thread_id] >= i && turn == thread_id) {
                // 다른 스레드가 현재 스레드의 우선 순위와 같은 경우 대기
                i--; // 우선 순위를 재설정
                break;
            }
        }
    }
}

// Dijkstra's algorithm: exit_critical_section
void exit_critical_section(int thread_id) {
    flag[thread_id] = 0; // 플래그를 0으로 설정
}

void* func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        enter_critical_section(thread_id); // Dijkstra's algorithm 진입
        printf("Thread[%d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        // printf("printf count: %d\n", cnt);
        exit_critical_section(thread_id); // Dijkstra's algorithm 종료
    }

    return NULL;
}

int main() {
    for (int i = 0; i < 101; i++) {
        n[i] = i;
    }

    pthread_t threads[N];
    args_t thread_args[N];

    // 각 스레드의 범위 설정
    int step = RANGE / N;
    for (int i = 0; i < N; i++) {
        thread_args[i].start = i * step + 1;
        thread_args[i].end = (i + 1) * step;
        thread_args[i].thread_id = i;

        // 스레드 생성
        pthread_create(&threads[i], NULL, func, &thread_args[i]);
    }

    // 스레드 종료 대기
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("printf count: %d\n", cnt);

    return 0;
}
