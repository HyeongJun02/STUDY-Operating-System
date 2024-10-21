#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define RANGE 100
#define THREAD_COUNT 4

// 세마포 객체
sem_t sem;

int n[101];
int cnt = 0;

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

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

int main() {
    for (int i = 0; i < 101; i++) {
        n[i] = i;
    }

    // 세마포 초기값
    sem_init(&sem, 0, 1);

    pthread_t threads[THREAD_COUNT];
    args_t thread_args[THREAD_COUNT];

    // thread 범위
    int step = RANGE / THREAD_COUNT;
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_args[i].start = i * step + 1;
        thread_args[i].end = (i + 1) * step;
        thread_args[i].thread_id = i;

        // thread create
        pthread_create(&threads[i], NULL, semaphore_func, &thread_args[i]);
    }

    // wait to end
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // 세마포 파괴!!
    sem_destroy(&sem);

    printf("Total count: %d\n", cnt);

    return 0;
}
