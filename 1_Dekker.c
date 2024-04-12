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
    flag[thread_id] = 1; // 스레드가 크리티컬 섹션에 들어갈 준비
    while (flag[1 - thread_id]) { // 상대 스레드가 진입을 시도하고 있는 동안
        if (turn != thread_id) { // 차례가 아니면
            flag[thread_id] = 0; // 진입을 시도하지 않고 대기
            while (turn != thread_id) {
                // 대기 루프
            }
            flag[thread_id] = 1; // 다시 진입 시도
        }
    }
}

void exit_critical_section(int thread_id) {
    turn = 1 - thread_id; // 차례를 상대 스레드로 전환
    flag[thread_id] = 0; // 진입 시도 종료
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
        exit_critical_section(thread_id); // Dekker's algorithm으로 종료
    }

    return NULL;
}

int main() {
    for(int i = 0; i < 101; i++)
        n[i] = i;

    pthread_t threads[2];
    args_t thread_args[2];

    // set thread 1
    thread_args[0].start = 1;
    thread_args[0].end = 50;
    thread_args[0].thread_id = 0;

    // set thread 2
    thread_args[1].start = 51;
    thread_args[1].end = 100;
    thread_args[1].thread_id = 1;

    // create threads
    pthread_create(&threads[0], NULL, func, &thread_args[0]);
    pthread_create(&threads[1], NULL, func, &thread_args[1]);

    // wait to end
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("printf count: %d\n", cnt);

    return 0;
}