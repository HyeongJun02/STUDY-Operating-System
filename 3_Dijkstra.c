#include <stdio.h>
#include <pthread.h>

#define N 4 // thread count
#define RANGE 100

// thread flag
enum state { IDLE = 0, WANT_IN, IN_CS };

int flag[N] = {IDLE}; // 각 스레드의 플래그 상태를 저장하는 배열
int turn = 0; // 턴을 나타내는 변수
int n[101]; // 1부터 100까지의 숫자를 저장하는 배열
int cnt = 0; // 출력된 수의 카운트

typedef struct {
    int start;
    int end;
    int thread_id;
} args_t;

void dijkstra_critical_section_enter(int thread_id) {
    // repeat 단계에서 loop 사용
    int j;
    do {
        // 임계 영역 진입 시도 1단계
        flag[thread_id] = WANT_IN;
        while (turn != thread_id) {
            if (flag[turn] == IDLE) {
                turn = thread_id;
            }
        }

        // 임계 영역 진입 시도 2단계
        flag[thread_id] = IN_CS;
        j = 0;
        while (j < N && (j == thread_id || flag[j] != IN_CS)) {
            j++;
        }
    } while (j < N); // 다른 스레드가 모두 IN_CS가 아니면 임계 영역 진입
}

void dijkstra_critical_section_exit(int thread_id) {
    // 임계 영역 종료
    flag[thread_id] = IDLE; // 플래그 상태를 IDLE로 설정
    turn = (turn + 1) % N; // 다음 턴으로 이동
}

void* dijkstra_func(void* args) {
    args_t* thread_args = (args_t*)args;
    int start = thread_args->start;
    int end = thread_args->end;
    int thread_id = thread_args->thread_id;

    for (int iter = start; iter <= end; iter++) {
        dijkstra_critical_section_enter(thread_id); // 임계 영역 진입
        printf("Thread[%d]: %3d * 3 = %3d\n", thread_id, n[iter], n[iter] * 3);
        cnt++;
        dijkstra_critical_section_exit(thread_id); // 임계 영역 종료
    }
    printf("=== thread[%2d] algorithm end ===\n", thread_id);

    return NULL;
}

int main() {
    // 숫자 배열 초기화
    for (int i = 0; i < 101; i++) {
        n[i] = i;
    }

    pthread_t threads[N];
    args_t thread_args[N];

    // 각 스레드의 범위 설정
    int step = RANGE / N;
    for (int i = 0; i < N; i++) {
        thread_args[i].start = i * step + 1;
        thread_args[i].end = (i + 1) * step;
        thread_args[i].thread_id = i;

        // 스레드 생성
        pthread_create(&threads[i], NULL, dijkstra_func, &thread_args[i]);
    }

    // wait to end
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // 최종 출력 카운트
    printf("printf count: %d\n", cnt);

    return 0;
}
