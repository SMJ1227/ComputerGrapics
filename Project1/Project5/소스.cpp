#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define ROW 30
#define COL 30

int board[ROW][COL] = { 0 };

void clearBoard() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            board[i][j] = 0;
        }
    }
}

void printBoard(int x1, int y1, int x2, int y2) {
    clearBoard();  // 출력하기 전에 보드를 초기화

    // 1. 정상적인 보드 내 영역을 그린다.
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            board[i % ROW][j % COL] = 1;  // 모듈로 연산을 통해 좌표 처리
        }
    }

    // 2. 경계를 넘어간 부분을 보드 반대편에 추가로 그린다.
    if (x1 > x2) {
        for (int i = x1; i < ROW; i++) {
            for (int j = y1; j <= y2; j++) {
                board[i % ROW][j % COL] = 1;
            }
        }
        for (int i = 0; i <= x2; i++) {
            for (int j = y1; j <= y2; j++) {
                board[i % ROW][j % COL] = 1;
            }
        }
    }

    if (y1 > y2) {
        for (int i = x1; i <= x2; i++) {
            for (int j = y1; j < COL; j++) {
                board[i % ROW][j % COL] = 1;
            }
        }
        for (int i = x1; i <= x2; i++) {
            for (int j = 0; j <= y2; j++) {
                board[i % ROW][j % COL] = 1;
            }
        }
    }

    // 보드 출력
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (board[i][j] == 0) {
                printf(" . ");
            }
            else {
                printf(" 0 ");
            }
        }
        printf("\n");
    }
}

int main() {
    int x1, x2, y1, y2;
    int temp;
    printf("점 두개를 입력하세요: ");
    scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

    // 항상 x1, y1이 작도록
    if (x1 > x2) {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y1 > y2) {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    printBoard(x1, y1, x2, y2);  // 처음 보드 그리기

    char input;
    while (1) {
        printf("명령어: ");
        scanf(" %c", &input);
        switch (input)
        {
        case 'x':    // 왼쪽으로 이동
            y1 = (y1 - 1 + COL) % COL;
            y2 = (y2 - 1 + COL) % COL;
            break;
        case 'X':    // 오른쪽으로 이동
            y1 = (y1 + 1) % COL;
            y2 = (y2 + 1) % COL;
            break;
        case 'y':    // 아래쪽으로 이동
            x1 = (x1 - 1 + ROW) % ROW;
            x2 = (x2 - 1 + ROW) % ROW;
            break;
        case 'Y':    // 위쪽으로 이동
            x1 = (x1 + 1) % ROW;
            x2 = (x2 + 1) % ROW;
            break;
        case 's':
            y2 = (y2 - 1 + COL) % COL;
            x2 = (x2 - 1 + ROW) % ROW;
            break;
        case 'S':
            y2 = (y2 + 1) % COL;
            x2 = (x2 + 1) % ROW;
            break;
        case 'i':
            y2 = (y2 - 1 + COL) % COL;
            break;
        case 'I':
            y2 = (y2 + 1) % COL;
            break;
        case 'j':
            x2 = (x2 - 1 + ROW) % ROW;
            break;
        case 'J':
            x2 = (x2 + 1) % ROW;
            break;
        case 'a':
            y2 = (y2 + 1) % COL;
            x2 = (x2 - 1 + ROW) % ROW;
            break;
        case 'A':
            y2 = (y2 - 1 + COL) % COL;
            x2 = (x2 + 1) % ROW;
            break;
        case 'm':
            printf("사각형의 면적: %d\n", ((x2-x1+1) * (y2-y1+1)));
            break;
        case 'n':
        {
            int rectWidth = (x2 - x1 + 1);
            int rectHeight = (y2 - y1 + 1);
            int rectArea = rectWidth * rectHeight;
            int boardArea = ROW * COL;

            double ratio = (double)rectArea / boardArea;
            printf("사각형 면적의 비율: %.2f\n", ratio*100);
        }
        break;

            break;
        case 'r':
            printf("점 두개를 입력하세요: ");
            scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

            // 항상 x1, y1이 작도록
            if (x1 > x2) {
                temp = x1;
                x1 = x2;
                x2 = temp;
            }

            if (y1 > y2) {
                temp = y1;
                y1 = y2;
                y2 = temp;
            }
            break;
        case 'q':
            return 0;
        }
        printBoard(x1, y1, x2, y2);  // 이동 후 보드 그리기
    }

    return 0;
}
