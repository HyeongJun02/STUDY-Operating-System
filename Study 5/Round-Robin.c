// 2142851 컴퓨터공학과 김형준
// Round-Robin

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

typedef struct Process {
    int id;                 // Process ID
    int multiplier;         // n X multiplier
    int running_time;       // Process Run Time
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
    } else {
        q->rear->next = process;
        q->rear = process;
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

<<<<<<< HEAD:Nonpreemptive_SJF.c
=======
int run_arrow = 0;
bool flag[THREAD_COUNT] = { true, false, false, false, false };
bool is_end[THREAD_COUNT] = { false, false, false, false, false };

void select_arrow(int id) {
    // printf("[TIME: %2d] SELECT ARROW FUNCTION START\n", total_time);
    int num;
    for (int i = 0; i < THREAD_COUNT; i++) {
        num = (id + i) % THREAD_COUNT;
        // printf("[TIME: %2d] i: %d\n", total_time, i);
        // printf("[TIME: %2d] id: %d\n", total_time, id);
        // printf("[TIME: %2d] num: %d\n", total_time, num);
        if (is_end[num]) {
            continue;
        }
        else {
            break;
        }
    }
    run_arrow = num;
    // printf("[TIME: %2d] run_arrow: %d\n", total_time, run_arrow);
    flag[run_arrow] = true;
}

>>>>>>> ce944cd11bf3d429658ed28a389fdfb505b63bb1:Round-Robin.c
void* processThread(void* arg) {
    Queue* q = (Queue*)arg;
    Process* process = dequeue(q);

    pthread_mutex_lock(&q->lock); // lock
    sprintf(gantt_chart + strlen(gantt_chart), "P%d (%d-", process->id, total_time);

    int start_time = total_time;
<<<<<<< HEAD:Nonpreemptive_SJF.c
    previous_time[process->id - 1] = next_processing_number[process->id - 1] - 1;
=======
    previous_time[process->id - 1] = 0;

    int running = 0;

>>>>>>> ce944cd11bf3d429658ed28a389fdfb505b63bb1:Round-Robin.c
    for (int i = next_processing_number[process->id - 1]; i <= process->running_time; i++) {
        usleep(10000); // 0.01 second delay
        printf("[TIME: %2d] P%d: %2d X %2d = %2d\n", total_time, process->id, i, process->multiplier, i * process->multiplier);
        total_time++;
        // printf("total_time : %d\n", total_time);
        next_processing_number[process->id - 1] = i + 1;

        running++;
        if (running == 5 || i == process->running_time) {
            flag[process->id - 1] = false;
            select_arrow(process->id);
            if (i == process->running_time) {
                // printf("[TIME: %2d] P%d END\n", total_time, process->id);
                // printf("[TIME: %2d] select_arrow: %d (P%d)\n", total_time, run_arrow, run_arrow + 1);
                is_end[process->id - 1] = true;
                break;
            }
            previous_time[process->id - 1] += running;
            sprintf(gantt_chart + strlen(gantt_chart), "%d)\n", total_time);

            pthread_mutex_unlock(&q->lock); // unlock

            while (!flag[process->id - 1]);

            pthread_mutex_lock(&q->lock); // lock
            
            running = 0;
            start_time = total_time;
            sprintf(gantt_chart + strlen(gantt_chart), "P%d (%d-", process->id, total_time);
        }
    }
    return_time[process->id - 1] = total_time - fixed_starting_time[process->id - 1];
    // 대기시간 = 마지막 작업 시작 시간(start_time) - 도착 시간(fixed_starting_time) - 이전 실행 시간의 합(previous_time)
<<<<<<< HEAD:Nonpreemptive_SJF.c
=======
    // printf("=========================================================\n");
    // printf("start_time: %d\n", start_time);
    // printf("fixed_starting_time[%d]: %d\n", process->id - 1, fixed_starting_time[process->id - 1]);
    // printf("previous_time[%d]: %d\n", process->id - 1, previous_time[process->id - 1]);
    // printf("=========================================================\n");
>>>>>>> ce944cd11bf3d429658ed28a389fdfb505b63bb1:Round-Robin.c
    waiting_time[process->id - 1] = start_time - fixed_starting_time[process->id - 1] - previous_time[process->id - 1];

    sprintf(gantt_chart + strlen(gantt_chart), "%d)\n", total_time);
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
        enqueue(&q, process);
        printQueue(&q);
    }

    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        while (total_time != fixed_starting_time[i]);
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
