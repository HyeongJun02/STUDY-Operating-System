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

    return NULL;
}

int main() {
    for(int i = 0; i < 101; i++)
        n[i] = i;

    pthread_t threads[2];
    args_t thread_args[2];

    // thread 0번
    thread_args[0].start = 1;
    thread_args[0].end = 50;
    thread_args[0].thread_id = 0;

    // thread 1번
    thread_args[1].start = 51;
    thread_args[1].end = 100;
    thread_args[1].thread_id = 1;

    // create threads
    pthread_create(&threads[0], NULL, dekker_func, &thread_args[0]);
    pthread_create(&threads[1], NULL, dekker_func, &thread_args[1]);

    // wait to end
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("printf count: %d\n", cnt);

    return 0;
}