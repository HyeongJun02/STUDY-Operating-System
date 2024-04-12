#include <stdio.h>
#include <pthread.h>

int flag[2] = {0, 0};
int turn = 0;
int n[101];
int cnt = 0;

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

void enter_critical_section(int thread_id) {
    flag[thread_id] = 1;
    turn = 1 - thread_id;
    while (flag[1 - thread_id] && turn == 1 - thread_id) {
        // 다른 스레드가 진입 중인 경우 대기
    }
}

void exit_critical_section(int thread_id) {
    flag[thread_id] = 0;
}

void* func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        enter_critical_section(thread_id); // Dekker's algorithm으로 진입
        printf("Thread[%2d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        exit_critical_section(thread_id);
    }

    return NULL;
}

int main() {
    for(int i = 0; i < 101; i++)
        n[i] = i;
    
    pthread_t threads[2];
    args_t thread_args[2];

    // Thread 0의 범위 설정
    thread_args[0].start = 1;
    thread_args[0].end = 50;
    thread_args[0].thread_id = 0;

    // Thread 1의 범위 설정
    thread_args[1].start = 51;
    thread_args[1].end = 100;
    thread_args[1].thread_id = 1;

    // 스레드 생성
    pthread_create(&threads[0], NULL, func, &thread_args[0]);
    pthread_create(&threads[1], NULL, func, &thread_args[1]);

    // 스레드 종료 대기
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("printf count: %d\n", cnt);

    return 0;
}
