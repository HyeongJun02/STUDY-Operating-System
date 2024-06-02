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

int main() {
    for(int i = 0; i < 101; i++)
        n[i] = i;
    
    pthread_t threads[2];
    args_t thread_args[2];

    // Thread 0번
    thread_args[0].start = 1;
    thread_args[0].end = 50;
    thread_args[0].thread_id = 0;

    // Thread 1번
    thread_args[1].start = 51;
    thread_args[1].end = 100;
    thread_args[1].thread_id = 1;

    // thread create
    pthread_create(&threads[0], NULL, peterson_func, &thread_args[0]);
    pthread_create(&threads[1], NULL, peterson_func, &thread_args[1]);

    // wait to end
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("printf count: %d\n", cnt);

    return 0;
}
