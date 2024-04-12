#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define RANGE 100 // 숫자 범위
#define THREAD_COUNT 4 // 스레드 수

// 세마포 객체 선언 및 초기화
sem_t sem;

int n[101]; // 1부터 100까지의 숫자를 저장하는 배열
int cnt = 0; // 출력된 수의 카운트

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

void* func(void* args) {
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

    return NULL;
}

int main() {
    for (int i = 0; i < 101; i++) {
        n[i] = i;
    }

    // 세마포 초기값 설정 (1)
    sem_init(&sem, 0, 1);

    pthread_t threads[THREAD_COUNT];
    args_t thread_args[THREAD_COUNT];

    // 각 스레드의 범위 설정
    int step = RANGE / THREAD_COUNT;
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_args[i].start = i * step + 1;
        thread_args[i].end = (i + 1) * step;
        thread_args[i].thread_id = i;

        // 스레드 생성
        pthread_create(&threads[i], NULL, func, &thread_args[i]);
    }

    // 스레드 종료 대기
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // 세마포 파괴
    sem_destroy(&sem);

    printf("Total count: %d\n", cnt);

    return 0;
}
