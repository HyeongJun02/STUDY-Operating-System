#include <stdio.h>
#include <stdlib.h>

#define REFSIZE 20
#define FRAMESIZE 3

typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    int capacity;
    int size;
    Node* front;
    Node* rear;
} Queue;

// Node 생성
Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

// Queue 생성
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = queue->rear = NULL;
    return queue;
}

// 큐가 전부 차 있는가?
int isFull(Queue* queue) {
    return queue->size == queue->capacity;
}

// 큐가 비어있는가?
int isEmpty(Queue* queue) {
    return queue->size == 0;
}

// enqueue
void enqueue(Queue* queue, int value) {
    Node* newNode = createNode(value);
    if (isFull(queue)) {
        Node* temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
        queue->size--;
    }
    if (isEmpty(queue)) {
        queue->front = queue->rear = newNode;
    }
    else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

// Queue 내의 요소 get
void getQueue(Queue* queue, int* arr) {
    Node* current = queue->front;
    int i = 0;
    while (current) {
        arr[i++] = current->value;
        current = current->next;
    }
    while (i < queue->capacity) {
        arr[i++] = 0;
    }
}

// Algorithm Function
void FIFO(int* ref, int** frames, int* page_faults, int* fault_count) {
    Queue* queue = createQueue(FRAMESIZE);
    *fault_count = 0; // page fault 시 page_faults 변수의 fault_count index에 시간 대입

    for (int i = 0; i < REFSIZE; i++) {
        int page = ref[i]; // page: 다음에 읽어올 문자열 (정수)
        int* arr = (int*)malloc(FRAMESIZE * sizeof(int));
        getQueue(queue, arr); // arr에 queue 요소 get

        int found = 0;
        for (int j = 0; j < FRAMESIZE; j++) {
            if (arr[j] == page) {
                found = 1;
                break;
            }
        }

        if (!found) {
            page_faults[(*fault_count)++] = i;
            enqueue(queue, page);
            getQueue(queue, arr);
        }

        for (int j = 0; j < FRAMESIZE; j++) {
            frames[i][j] = arr[j];
        }
        free(arr);
    }
}

int main() {
    int ref[REFSIZE] = { 1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3 };

    int** frames = (int**)malloc(REFSIZE * sizeof(int*));
    for (int i = 0; i < REFSIZE; i++) {
        frames[i] = (int*)malloc(FRAMESIZE * sizeof(int));
        for (int j = 0; j < FRAMESIZE; j++) {
            frames[i][j] = 0;
        }
    }

    // page_faults를 일단 최대 크기로 설정
    int* page_faults = (int*)malloc(REFSIZE * sizeof(int));
    int fault_count = 0;

    FIFO(ref, frames, page_faults, &fault_count);

    printf("<Queue 상태>\n");
    for (int i = 0; i < REFSIZE; i++) {
        int isFault = 0;
        for (int j = 0; j < fault_count; j++) {
            if (i == page_faults[j]) {
                isFault = 1;
                break;
            }
        }
        if (isFault) {
            printf("* ");
        }
        else {
            printf("  ");
        }
        printf("[TIME: %2d] ", i);
        for (int j = 0; j < FRAMESIZE; j++) {
            printf("%d ", frames[i][j]);
        }
        printf("\n");
    }

    printf("\n<페이지 부재 - %2d번>\n", fault_count);
    for (int i = 0; i < fault_count; i++) {
        printf("%d ", page_faults[i]);
    }
    printf("\n");

    for (int i = 0; i < REFSIZE; i++) {
        free(frames[i]);
    }
    free(frames);
    free(page_faults);

    return 0;
}
