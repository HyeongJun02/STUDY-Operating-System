// 2142851 컴퓨터공학과 김형준
// Preemptive_SJF

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#define THREAD_COUNT 5

char gantt_chart[300];

static int fixed_running_time[THREAD_COUNT] = { 10, 28, 6, 4, 14 };
static int fixed_starting_time[THREAD_COUNT] = { 0, 1, 2, 3, 4 };

int return_time[THREAD_COUNT] = { 0, };     // 반환 시간
int waiting_time[THREAD_COUNT] = { 0, };    // 대기 시간
int total_time = 0;                         // 전체 시간

int next_processing_number[THREAD_COUNT] = { 1, 1, 1, 1, 1 };   // 다음으로 실행할 시간
int previous_time[THREAD_COUNT];                                // 이전까지 실행한 시간

bool flag[THREAD_COUNT] = { true, false, false, false, false };

int now = 0;

typedef struct Process {
    int id;                 // Process ID
    int multiplier;         // n X multiplier
    int running_time;       // Process Run Time
    int start_time;
    struct Process* next;
} Process;

typedef struct Queue {
    Process* front;
    Process* rear;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} Queue;

Queue process_q;

// print
void printQueue(Queue* q) {
    pthread_mutex_lock(&q->lock);
    Process* current = q->front;
    printf("============================ <Queue> ============================\n");
    if (current == NULL) printf("|\t\t\tCLEAR\t\t\t\t\t|\n");
    while (current != NULL) {
        printf("|Process ID: %d, Multiplier: %d, Start Time: %d, Running Time: %d\t|\n", current->id, current->multiplier, current->start_time, current->running_time);
        current = current->next;
    }
    printf("=================================================================\n");
    pthread_mutex_unlock(&q->lock);
}

// init
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void enqueue(Queue* q, Process* process) {
    pthread_mutex_lock(&q->lock);
    if (q->rear == NULL) {
        q->front = q->rear = process;
    }
    else {
        Process* current = q->front;
        // current->next의 running_time이 process의 running_time보다 짧을 경우까지 반복
        while (current->next != NULL && current->next->running_time < process->running_time) {
            // current를 현재의 다음으로 설정
            current = current->next;
        }
        process->next = current->next;
        current->next = process;
        // 삽입된 process의 다음이 NULL일 경우
        // (마지막 순번이라면)
        if (process->next == NULL) {
            q->rear = process;
        }
    }
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

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

// // new function (dequeue and enqueue)
// void dequeue_and_enqueue(Queue* q) {
//     pthread_mutex_lock(&q->lock);

//     if (q->front == NULL) {
//         pthread_mutex_unlock(&q->lock);
//         return; // Queue가 비어있으면 아무 작업도 하지 않음
//     }

//     Process* front_process = q->front;
//     q->front = q->front->next;

//     // Queue의 끝에 삽입하기 위해 rear를 찾음
//     if (q->rear == NULL) {
//         q->rear = front_process;
//     } else {
//         q->rear->next = front_process;
//         q->rear = front_process;
//     }

//     front_process->next = NULL; // 다음 포인터를 NULL로 설정하여 새로운 Queue의 끝을 표시함

//     pthread_mutex_unlock(&q->lock);
// }

void* processThread(void* arg) {
    Queue* q = (Queue*)arg;
    Process* process = dequeue(q);
    printf("[P%d] dequeue(q)\n", process->id);
    printQueue(q);

    enqueue(q, process);
    printf("[P%d] enqueue(q)\n", process->id);
    printQueue(q);

    // 전부 처리된 process라면
    if (process->start_time > process->running_time) {
        dequeue(q);
        free(process);
        return NULL;
    }

    pthread_mutex_lock(&q->lock); // lock
    now = process->id - 1;
    sprintf(gantt_chart + strlen(gantt_chart), "P%d (%d-", process->id, total_time);

    process->start_time = total_time;
    previous_time[process->id - 1] = next_processing_number[process->id - 1] - 1;
    for (int i = next_processing_number[process->id - 1]; i <= process->running_time; i++) {
        // printf("%d\n", i);
        // Preemptive
        if (!flag[process->id - 1]) {
            // sprintf(gantt_chart + strlen(gantt_chart), "%d)\n", total_time);
            // pthread_mutex_unlock(&q->lock); // unlock
            break;
        }
        // usleep(10000); // 0.01 second delay
        printf("[TIME: %2d] P%d: %2d X %2d = %2d\n", total_time, process->id, i, process->multiplier, i * process->multiplier);
        total_time++;
        usleep(10000); // 0.01 second delay
        // printf("total_time : %d\n", total_time);
        next_processing_number[process->id - 1] = i + 1;
    }
    return_time[process->id - 1] = total_time - fixed_starting_time[process->id - 1];
    // 대기시간 = 마지막 작업 시작 시간(process->start_time) - 도착 시간(fixed_starting_time) - 이전 실행 시간의 합(previous_time)
    waiting_time[process->id - 1] = process->start_time - fixed_starting_time[process->id - 1] - previous_time[process->id - 1];

    sprintf(gantt_chart + strlen(gantt_chart), "%d)\n", total_time);
    pthread_mutex_unlock(&q->lock); // unlock

    printf("[P%d] free and gantt_chart\n%s\n", process->id, gantt_chart);
    dequeue(q);
    free(process);
    return NULL;
}

int main() {
    initQueue(&process_q);

    for (int i = 0; i < THREAD_COUNT; i++) {
        Process* process = (Process*)malloc(sizeof(Process));
        process->id = i + 1;
        process->multiplier = i + 1;
        process->next = NULL;
        process->running_time = fixed_running_time[i];
        process->start_time = 0;
        enqueue(&process_q, process);
        printQueue(&process_q);
    }

    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        while (total_time != fixed_starting_time[i]);
        printf("[TIME: %2d] P%d is arrived\n", total_time, i + 1);
        // Preemptive
        if (fixed_running_time[i] < fixed_running_time[now]) {
            printf("[TIME: %2d] fixed_running_time[P%d](%d) < fixed_running_time[P%d](%d)\n", total_time, i + 1, fixed_running_time[i], now + 1, fixed_running_time[now]);
            flag[now] = false;
            flag[i] = true;
            now = i;
            printQueue(&process_q);
        }
        pthread_create(&threads[i], NULL, processThread, &process_q);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("END\n");
    printQueue(&process_q);
    printf("===========================================================\n");
    printf("Process\t|\tReturn Time\t|\tWaiting Time\n");
    printf("-----------------------------------------------------------\n");
    double sum_return_time = 0, sum_waiting_time = 0;
    for (int i = 0; i < THREAD_COUNT; i++) {
        printf("P%d\t|\t%d\t\t|\t%d\n", i + 1, return_time[i], waiting_time[i]);
        sum_return_time += return_time[i];
        sum_waiting_time += waiting_time[i];
    }
    printf("===========================================================\n");
    printf("Result\t|\t평균 반환시간\t|\t평균 대기시간\n");
    printf("-----------------------------------------------------------\n");
    printf("-\t|\t%.1lf\t\t|\t%.1lf\n", sum_return_time / THREAD_COUNT, sum_waiting_time / THREAD_COUNT);
    printf("===========================================================\n");
    printf("Gantt Chart\n");
    printf("-----------------------------------------------------------\n");
    printf("%s", gantt_chart);
    printf("===========================================================\n");

    return 0;
}
