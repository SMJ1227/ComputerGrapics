#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> // _kbhit()�� _getch() ����� ���� ���

#define SIZE 30
#define PATH 0
#define BACKGROUND 1
#define OBSTACLE 'X'

int grid[SIZE][SIZE];

// ��ֹ� ����
void create_obstacles() {
    int obstacles = 0;
    while (obstacles < 30) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;
        if (grid[x][y] == BACKGROUND) {
            grid[x][y] = OBSTACLE;
            obstacles++;
        }
    }
}

// ��� ����
int create_path() {
    int x = 0, y = 0;
    grid[x][y] = PATH; // ������

    int direction = 0; // 0: ������, 1: �Ʒ�, 2: ����, 3: ��
    int steps = 0;
    
    while (1) { // ���� ���� ���
        if (x == SIZE - 1 && y == SIZE - 1) break; // ���� ���� Ȯ��

        if (steps < 5) {
            // �̵�
            if (direction == 0 && y < SIZE - 1 && grid[x][y + 1] != OBSTACLE) { // ������
                y++;
            }
            else if (direction == 1 && x < SIZE - 1 && grid[x + 1][y] != OBSTACLE) { // �Ʒ�
                x++;
            }
            else if (direction == 2 && y > 0 && grid[x][y - 1] != OBSTACLE) { // ����
                y--;
            }
            else if (direction == 3 && x > 0 && grid[x - 1][y] != OBSTACLE) { // ��
                x--;
            }
            else {
                // ��ֹ��� �ε����� �� ���� ��ȯ
                int original_direction = direction; // ���� ���� ����
                // ���ο� �������� ��ȯ
                do {
                    direction = (direction + 1) % 4; // ���� �������� ��ȯ
                } while (direction == original_direction); // ���� ����� �������� ���� ������
                continue; // ���� �ݺ����� �̵�
            }
            grid[x][y] = PATH; // ��� ǥ��
            steps++; // �̵��� ���� ī��Ʈ ����
        }
        else {
            // �ִ� 5ĭ �̵� �� ���� ��ȯ
            direction = (direction + 1) % 4;
            steps = 0; // ���� ī��Ʈ ����
        }
    }
    return 0;
}



// ȭ�� ���
void print_grid() {
    system("cls"); // ȭ�� �����
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == BACKGROUND) printf("1 ");
            else if (grid[i][j] == OBSTACLE) printf("X ");
            else printf("0 ");
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));

    // �ʱ�ȭ
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = BACKGROUND;

    create_obstacles();
    create_path();
    print_grid();

    return 0;
}
