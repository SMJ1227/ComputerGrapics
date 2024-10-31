#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> // _kbhit()와 _getch() 사용을 위한 헤더

#define SIZE 30
#define PATH 0
#define BACKGROUND 1
#define OBSTACLE 'X'

int grid[SIZE][SIZE];

// 장애물 생성
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

// 경로 생성
int create_path() {
    int x = 0, y = 0;
    grid[x][y] = PATH; // 시작점

    int direction = 0; // 0: 오른쪽, 1: 아래, 2: 왼쪽, 3: 위
    int steps = 0;
    
    while (1) { // 무한 루프 사용
        if (x == SIZE - 1 && y == SIZE - 1) break; // 도착 지점 확인

        if (steps < 5) {
            // 이동
            if (direction == 0 && y < SIZE - 1 && grid[x][y + 1] != OBSTACLE) { // 오른쪽
                y++;
            }
            else if (direction == 1 && x < SIZE - 1 && grid[x + 1][y] != OBSTACLE) { // 아래
                x++;
            }
            else if (direction == 2 && y > 0 && grid[x][y - 1] != OBSTACLE) { // 왼쪽
                y--;
            }
            else if (direction == 3 && x > 0 && grid[x - 1][y] != OBSTACLE) { // 위
                x--;
            }
            else {
                // 장애물에 부딪혔을 때 방향 전환
                int original_direction = direction; // 현재 방향 저장
                // 새로운 방향으로 전환
                do {
                    direction = (direction + 1) % 4; // 다음 방향으로 전환
                } while (direction == original_direction); // 현재 방향과 동일하지 않을 때까지
                continue; // 다음 반복으로 이동
            }
            grid[x][y] = PATH; // 경로 표시
            steps++; // 이동한 스텝 카운트 증가
        }
        else {
            // 최대 5칸 이동 후 방향 전환
            direction = (direction + 1) % 4;
            steps = 0; // 스텝 카운트 리셋
        }
    }
    return 0;
}



// 화면 출력
void print_grid() {
    system("cls"); // 화면 지우기
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

    // 초기화
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = BACKGROUND;

    create_obstacles();
    create_path();
    print_grid();

    return 0;
}
