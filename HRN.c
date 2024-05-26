// 2142851 컴퓨터공학과 김형준
// HRN

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#define THREAD_COUNT 5

char gantt_chart[300];

static int fixed_running_time[THREAD_COUNT] = { 10, 28, 6, 4, 14 };
static int fixed_starting_time[THREAD_COUNT] = { 0, 1, 2, 3, 4 };

int priority[THREAD_COUNT] = { 3, 2, 4, 1, 2 };

int return_time[THREAD_COUNT] = { 0, };     // 반환 시간
int waiting_time[THREAD_COUNT] = { 0, };    // 대기 시간
int total_time = 0;                         // 전체 시간

int next_processing_number[THREAD_COUNT] = { 1, 1, 1, 1, 1 };   // 다음으로 실행할 시간
int previous_time[THREAD_COUNT];                                // 이전까지 실행한 시간

typedef struct Process {
    int id;                 // Process ID
    int multiplier;         // n X multiplier
    int running_time;       // Process Run Time
    int priority;
    struct Process* next;
} Process;

typedef struct Queue {
    Process* front;
    Process* rear;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} Queue;

// print
void printQueue(Queue* q) {
    pthread_mutex_lock(&q->lock);
    Process* current = q->front;
    printf("============================ <Queue> ============================\n");
    if (current == NULL) printf("|\t\t\tCLEAR\t\t\t\t\t|\n");
    while (current != NULL) {
        printf("|\tProcess ID: %d, Multiplier: %d, Running Time: %d\t\t|\n", current->id, current->multiplier, current->running_time);
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
        // sort loop
        while (current->next != NULL && current->next->priority >= process->priority) {
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

bool flag[THREAD_COUNT] = { true, false, false, false, false };
bool is_end[THREAD_COUNT] = { false, false, false, false, false };

void flag_func() {
    int waited_time[THREAD_COUNT];
    int next_id = 0;
    double value;
    int max = 0;

    for (int i = 0; i < THREAD_COUNT; i++) {
        if (is_end[i]) continue;
        waited_time[i] = total_time - fixed_starting_time[i];
        value = 1 + (1.0 * waited_time[i]/fixed_running_time[i]);
        if (max < value) {
            max = value;
            next_id = i;
        }
        printf("P%d: %lf\n", i + 1, value);
    }

    printf("next_id: %d\n", next_id);
    flag[next_id] = true;
}

void* processThread(void* arg) {
    Queue* q = (Queue*)arg;
    Process* process = dequeue(q);

    while (!flag[process->id - 1]);
    pthread_mutex_lock(&q->lock); // lock
    sprintf(gantt_chart + strlen(gantt_chart), "P%d (%d-", process->id, total_time);

    int start_time = total_time;
    previous_time[process->id - 1] = next_processing_number[process->id - 1] - 1;
    for (int i = next_processing_number[process->id - 1]; i <= process->running_time; i++) {
        usleep(10000); // 0.01 second delay
        printf("[TIME: %2d] P%d: %2d X %2d = %2d\n", total_time, process->id, i, process->multiplier, i * process->multiplier);
        total_time++;
        // printf("total_time : %d\n", total_time);
        next_processing_number[process->id - 1] = i + 1;
    }
    return_time[process->id - 1] = total_time - fixed_starting_time[process->id - 1];
    // 대기시간 = 마지막 작업 시작 시간(start_time) - 도착 시간(fixed_starting_time) - 이전 실행 시간의 합(previous_time)
    waiting_time[process->id - 1] = start_time - fixed_starting_time[process->id - 1] - previous_time[process->id - 1];

    sprintf(gantt_chart + strlen(gantt_chart), "%d)\n", total_time);
    is_end[process->id - 1] = true;
    flag[process->id - 1] = false;
    flag_func();
    pthread_mutex_unlock(&q->lock); // unlock

    free(process);
    return NULL;
}

int main() {
    Queue q;
    initQueue(&q);

    for (int i = 0; i < THREAD_COUNT; i++) {
        Process* process = (Process*)malloc(sizeof(Process));
        process->id = i + 1;
        process->multiplier = i + 1;
        process->next = NULL;
        process->running_time = fixed_running_time[i];
        process->priority = priority[i];
        enqueue(&q, process);
        printQueue(&q);
    }

    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        while (total_time < fixed_starting_time[i]);
        printf("[TIME: %2d] P%d is arrived\n", total_time, i + 1);
        pthread_create(&threads[i], NULL, processThread, &q);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("END\n");
    printQueue(&q);
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

