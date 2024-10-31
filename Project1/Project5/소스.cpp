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
    clearBoard();  // ����ϱ� ���� ���带 �ʱ�ȭ

    // 1. �������� ���� �� ������ �׸���.
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            board[i % ROW][j % COL] = 1;  // ���� ������ ���� ��ǥ ó��
        }
    }

    // 2. ��踦 �Ѿ �κ��� ���� �ݴ��� �߰��� �׸���.
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

    // ���� ���
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
    printf("�� �ΰ��� �Է��ϼ���: ");
    scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

    // �׻� x1, y1�� �۵���
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

    printBoard(x1, y1, x2, y2);  // ó�� ���� �׸���

    char input;
    while (1) {
        printf("��ɾ�: ");
        scanf(" %c", &input);
        switch (input)
        {
        case 'x':    // �������� �̵�
            y1 = (y1 - 1 + COL) % COL;
            y2 = (y2 - 1 + COL) % COL;
            break;
        case 'X':    // ���������� �̵�
            y1 = (y1 + 1) % COL;
            y2 = (y2 + 1) % COL;
            break;
        case 'y':    // �Ʒ������� �̵�
            x1 = (x1 - 1 + ROW) % ROW;
            x2 = (x2 - 1 + ROW) % ROW;
            break;
        case 'Y':    // �������� �̵�
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
            printf("�簢���� ����: %d\n", ((x2-x1+1) * (y2-y1+1)));
            break;
        case 'n':
        {
            int rectWidth = (x2 - x1 + 1);
            int rectHeight = (y2 - y1 + 1);
            int rectArea = rectWidth * rectHeight;
            int boardArea = ROW * COL;

            double ratio = (double)rectArea / boardArea;
            printf("�簢�� ������ ����: %.2f\n", ratio*100);
        }
        break;

            break;
        case 'r':
            printf("�� �ΰ��� �Է��ϼ���: ");
            scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

            // �׻� x1, y1�� �۵���
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
        printBoard(x1, y1, x2, y2);  // �̵� �� ���� �׸���
    }

    return 0;
}
