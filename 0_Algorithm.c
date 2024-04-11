#include <stdio.h>

void Dekker() {
    flag[0] = false;
    flag[1] = false;
    turn = 0;

    flag[0] = true;
    while (flag[1] == true) {
        if (turn == 1) {
            flag[0] = false;
            while(turn == 1) {
                // 바쁜 대기
            }
            flag[0] = true;
        }
    }
    /* 임계 영역 */
    turn = 1;
    flag[0] = false;
    /* 나머지 영역 */

    // 프로세스 P1
    flag[1] = true;
    while (flag[0] == true) {
        if (turn == 0) {
            flag[1] = false;
            while (turn == 0) {
                // 바쁜 대기
            }
            flag[1] = true;
        }
    }
    /* 임계 영역 */
    turn = 0;
    flag[1] = false;
    /* 나머지 영역 */
}