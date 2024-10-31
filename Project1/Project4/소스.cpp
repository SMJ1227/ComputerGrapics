#define _CRT_SECURE_NO_WARNINGS
#define ROW 5
#define COL 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>

int board[5][5];
int isShowing[5][5] = { 0 };
int color = 2;
int defalutColor = 15;

void initBoard() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            isShowing[i][j] = 0;
        }
    }
    char letters[25] = { 'A', 'A', 'B', 'B', 'C', 'C', 'D', 'D', 'E', 'E',
                         'F', 'F', 'G', 'G', 'H', 'H', 'I', 'I', 'M', 'M',
                         'K', 'K', 'L', 'L', 'J' }; // J는 조커

    srand((unsigned int)time(NULL));

    for (int i = 24; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp = letters[i];
        letters[i] = letters[j];
        letters[j] = temp;
    }

    int index = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            board[i][j] = letters[index++];
        }
    }
}

void printBoard() {
    printf("    a  b  c  d  e ");
    for (int i = 0; i < ROW; i++) {
        printf("\n %d ", i + 1);
        for (int j = 0; j < COL; j++) {
            if (isShowing[i][j] == 1) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
                printf(" %c ", board[i][j]);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defalutColor);
            }
            else {
                printf(" * ");
            }
        }
    }
    printf("\n\n");
}


int main() {
	initBoard();
	printBoard();

    int turn = 10;
    int score = 0;

    while (1) {
        char column1;
        int row1;
        char column2;
        int row2;
        char answer1 = 'z';
        char answer2 = 'y';

        printf("(턴: %d/ 스코어: %d) 좌표 입력: ", turn, score);
        scanf(" %c%d %c%d", &column1, &row1, &column2, &row2);
        if (column1 == 'r' || column2 == 'r') { initBoard(); turn = 10; score = 0; continue; }
        if (column1 == 'q' || column2 == 'q') { break; }

        int colIndex1 = column1 - 'a';
        int rowIndex1 = row1 - 1;
        
        if (colIndex1 >= 0 && colIndex1 < COL && rowIndex1 >= 0 && rowIndex1 < ROW && isShowing[rowIndex1][colIndex1] != 1) {
            isShowing[rowIndex1][colIndex1] = 1;
            answer1 = board[rowIndex1][colIndex1];
        }
        else {
            printf("잘못된 좌표입니다. 다시 입력해주세요.\n");
            answer1 = 'z';
        }
        
        int colIndex2 = column2 - 'a';
        int rowIndex2 = row2 - 1;

        if (colIndex2 >= 0 && colIndex2 < COL && rowIndex2 >= 0 && rowIndex2 < ROW && isShowing[rowIndex2][colIndex2] != 1) {
            isShowing[rowIndex2][colIndex2] = 1;
            answer2 = board[rowIndex2][colIndex2];
        }
        else {
            printf("잘못된 좌표입니다. 다시 입력해주세요.\n");
            answer2 = 'y';
        }

        // 비교
        if (answer1 == answer2) {
            printBoard();
            score += 100;
            color++;
        }
        else if (answer1 == 'J') {
            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++) {
                    if (board[i][j] == answer2 && isShowing[i][j] == 0) {
                        isShowing[i][j] = 1;
                    }
                }
            }
            printBoard();
            score += 100;
            turn--;
            color++;
            if (turn == 0) {
                printf("게임 종료. 최종 점수: %d", score);
                break;
            }
        }
        else if (answer2 == 'J') {
            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++) {
                    if (board[i][j] == answer1 && isShowing[i][j] == 0) {
                        isShowing[i][j] = 1;
                    }
                }
            }
            printBoard();
            score += 100;
            turn--;
            color++;
            if (turn == 0) {
                printf("게임 종료. 최종 점수: %d", score);
                break;
            }
        }

        else {
            printBoard();
            isShowing[rowIndex1][colIndex1] = 0;
            isShowing[rowIndex2][colIndex2] = 0;
            turn--;
            if (turn == 0) {
                printf("게임 종료. 최종 점수: ", score);
                break;
            }
        }
    }
	return 0;
}