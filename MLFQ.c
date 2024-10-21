#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESS_COUNT 3
#define Q1_TIME 1
#define Q2_TIME 2
#define Q3_TIME 4

typedef struct {
    int id;                 // ID
    int burst_time;         // 실행 시간
    int remaining_time;     // 남은 실행 시간
    int arrival_time;       // 도착 시간
    int return_time;        // 반환 시간 (종료 - 도착)
    int waiting_time;       // 대기 시간 (반환 - 실행)
    int tmp;
} Process;

/* ========================================== Queue Func Start ========================================== */
typedef struct {
    Process* queue[PROCESS_COUNT];
    int front;
    int rear;
} Queue;

void initQueue(Queue* q) {
    q->front = q->rear = -1;
}

bool isEmpty(Queue* q) {
    return q->front == -1;
}

void enqueue(Queue* q, Process* p) {
    if (q->rear == PROCESS_COUNT - 1) return;
    if (q->front == -1) q->front = 0;
    q->queue[++q->rear] = p;
}

Process* dequeue(Queue* q) {
    if (isEmpty(q)) return NULL;
    Process* p = q->queue[q->front];
    if (q->front >= q->rear) q->front = q->rear = -1;
    else q->front++;
    return p;
}
/* ========================================== Queue Func End ========================================== */

int tmp1 = 1;
int tmp2 = 1;
int tmp3 = 1;

/* ========================================== <MLFQ> Start ========================================== */
void MLFQ(Process processes[], int n) {
    Queue q1, q2, q3;
    initQueue(&q1);
    initQueue(&q2);
    initQueue(&q3);
    
    for (int i = 0; i < n; i++) {
        enqueue(&q1, &processes[i]);
    }

    int time = 0;
    char gantt_chart[1000] = "";
    char tmp_gantt_chart[500] = "";
    
    while (!isEmpty(&q1) || !isEmpty(&q2) || !isEmpty(&q3)) {
        Process* current = NULL;
        int time_quantum = 0;

        if (!isEmpty(&q1)) {
            current = dequeue(&q1);
            time_quantum = Q1_TIME;
        } else if (!isEmpty(&q2)) {
            current = dequeue(&q2);
            time_quantum = Q2_TIME;
        } else if (!isEmpty(&q3)) {
            current = dequeue(&q3);
            time_quantum = Q3_TIME;
        }

        if (current != NULL) {
            // if (current->remaining_time > time_quantum) {
            //     exec_time = time_quantum;
            // }
            // else {
            //     exec_time = current->remaining_time;
            // }
            int exec_time = (current->remaining_time > time_quantum) ? time_quantum : current->remaining_time;
            sprintf(tmp_gantt_chart + strlen(tmp_gantt_chart), "P%d (%d-%d) ", current->id, time, time + exec_time);
            time += exec_time;
            current->remaining_time -= exec_time;
            printf("[TIME: %2d] [P%d] remaining_time: %2d\n", time, current->id, current->remaining_time);

            // fork!!
            pid_t pid = fork();

            if (pid == 0) {
                switch (current->id) {
                    case 1:
                        printf("[TIME: %2d] P%d: %d x %d = %d\n", time, current->id, current->tmp, current->id, current->tmp * current->id);
                        current->tmp++;
                        break;
                    case 2:
                        printf("[TIME: %2d] P%d: %d x %d = %d\n", time, current->id, current->tmp, current->id, current->tmp * current->id);
                        current->tmp++;
                        break;
                    case 3:
                        printf("[TIME: %2d] P%d: %d x %d = %d\n", time, current->id, current->tmp, current->id, current->tmp * current->id);
                        current->tmp++;
                        break;
                }
                exit(0);
            } else if (pid > 0) {
                wait(NULL);
            }
            // printf("--------------------------------------------------------------------------\n");
            // printf("[TIME: %2d] Cycle clear\n", time);
            // printf("--------------------------------------------------------------------------\n");
            
            if (current->remaining_time == 0) {
                current->return_time = time - current->arrival_time;
                current->waiting_time = current->return_time - current->burst_time;
            } else {
                if (current->remaining_time > 1 && exec_time == 1) {
                    sprintf(gantt_chart + strlen(gantt_chart), "Q1: %s\n", tmp_gantt_chart);
                    strcpy(tmp_gantt_chart, "");
                    enqueue(&q2, current);
                }
                else if (current->remaining_time > 2) {
                    sprintf(gantt_chart + strlen(gantt_chart), "Q2: %s\n", tmp_gantt_chart);
                    strcpy(tmp_gantt_chart, "");
                    enqueue(&q3, current);
                } else if (current->remaining_time > 1) {
                    sprintf(gantt_chart + strlen(gantt_chart), "Q1: %s\n", tmp_gantt_chart);
                    strcpy(tmp_gantt_chart, "");
                    enqueue(&q2, current);
                } else {
                    sprintf(gantt_chart + strlen(gantt_chart), "Q3: %s\n", tmp_gantt_chart);
                    strcpy(tmp_gantt_chart, "");
                    enqueue(&q1, current);
                }
            }
        }
    }

    double total_return_time = 0;
    double total_waiting_time = 0;

    printf("===========================================================\n");
    printf("Process\t|\tReturn Time\t|\tWaiting Time\n");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t|\t%d\t\t|\t%d\n", processes[i].id, processes[i].return_time, processes[i].waiting_time);
        total_return_time += processes[i].return_time;
        total_waiting_time += processes[i].waiting_time;
    }
    printf("===========================================================\n");
    printf("Result\t|\t평균 반환시간\t|\t평균 대기시간\n");
    printf("-----------------------------------------------------------\n");
    printf("-\t|\t%.1lf\t\t|\t%.1lf\n", total_return_time / n, total_waiting_time / n);
    printf("===========================================================\n");
    printf("Gantt Chart\n");
    printf("-----------------------------------------------------------\n");
    printf("%s\n", gantt_chart);
    printf("===========================================================\n");
}
/* ========================================== <MLFQ> End ========================================== */

int main() {
    Process processes[PROCESS_COUNT] = {
        {1, 30, 30, 0, 0, 0, 1},
        {2, 20, 20, 0, 0, 0, 1},
        {3, 10, 10, 0, 0, 0, 1}
    };

    MLFQ(processes, PROCESS_COUNT);
    return 0;
}
