#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_COUNT 5

// 각 프로세스를 표현하는 구조체
typedef struct Process {
    int id;             // 프로세스 ID
    int multiplier;     // 프로세스가 사용할 곱수
    int running_time;   // Process Run Time
    struct Process* next; // 다음 프로세스를 가리키는 포인터
} Process;

// 큐를 표현하는 구조체
typedef struct Queue {
    Process* front; // 큐의 첫 번째 요소를 가리키는 포인터
    Process* rear; // 큐의 마지막 요소를 가리키는 포인터
    pthread_mutex_t lock; // 큐에 대한 뮤텍스
    pthread_cond_t not_empty; // 큐가 비어있지 않음을 나타내는 조건 변수
} Queue;

// 큐의 내용을 출력하는 함수
void printQueue(Queue* q) {
    pthread_mutex_lock(&q->lock);
    Process* current = q->front;
    printf("Queue Contents:\n");
    while (current != NULL) {
        printf("Process ID: %d, Multiplier: %d, Running Time: %d\n", current->id, current->multiplier, current->running_time);
        current = current->next;
    }
    pthread_mutex_unlock(&q->lock);
}

// 큐 초기화 함수
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

// 큐에 요소 추가 함수
void enqueue(Queue* q, Process* process) {
    pthread_mutex_lock(&q->lock);
    if (q->rear == NULL) {
        q->front = q->rear = process;
    } else {
        q->rear->next = process;
        q->rear = process;
    }
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

// 큐에서 요소 제거 함수
Process* dequeue(Queue* q) {
    pthread_mutex_lock(&q->lock);
    while (q->front == NULL) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    Process* process = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    pthread_mutex_unlock(&q->lock);
    return process;
}

// 프로세스를 처리하는 스레드 함수
void* processThread(void* arg) {
    Queue* q = (Queue*)arg;
    Process* process = dequeue(q); // 큐에서 프로세스 가져오기
    pthread_mutex_lock(&q->lock);
    for (int i = 1; i <= process->running_time; i++) {
        usleep(10000); // 0.01 second delay
        printf("P%d: %d X %d = %d\n", process->id, i, process->multiplier, i * process->multiplier);
    }
    pthread_mutex_unlock(&q->lock);
    free(process); // 메모리 해제
    return NULL;
}

int main() {
    // 큐 초기화
    Queue q;
    initQueue(&q);

    // 각 프로세스 생성 및 큐에 추가
    for (int i = 1; i <= THREAD_COUNT; i++) {
        Process* process = (Process*)malloc(sizeof(Process));
        process->id = i;
        process->multiplier = i;
        process->next = NULL;
        if (i == 1) process->running_time = 10;
        else if (i == 2) process->running_time = 28;
        else if (i == 3) process->running_time = 6;
        else if (i == 4) process->running_time = 4;
        else if (i == 5) process->running_time = 14;
        enqueue(&q, process);
        printQueue(&q);
    }

    // 스레드 생성
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, processThread, &q);
    }

    // 스레드 종료 대기
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
