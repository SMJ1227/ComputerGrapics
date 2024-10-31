#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#define WINDOW_X 800
#define WINDOW_Y 600

GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat rectSize = 0.8f;

int clickRect = -1;
GLfloat offsetX = 0.0f, offsetY = 0.0f;

typedef struct {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
    int type;
    int moving;
} Rect;

int rectCount = 5;
Rect rectangles[5];

// 랜덤 색상 생성 함수
void randomizeColor(Rect* rect) {
    rect->color[0] = (GLfloat)rand() / RAND_MAX;
    rect->color[1] = (GLfloat)rand() / RAND_MAX;
    rect->color[2] = (GLfloat)rand() / RAND_MAX;
}

// 사각형 내부/외부 클릭 확인 함수
int isInsideRect(int x, int y, Rect* rect) {
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y); // 윈도우 좌표에서 OpenGL 좌표 변환

    return (normalizedX >= rect->x1 && normalizedX <= rect->x2 &&
        normalizedY >= rect->y1 && normalizedY <= rect->y2);
}

void splitRectangle(int index) {
    Rect r = rectangles[index];

    GLfloat halfWidth = (r.x2 - r.x1) / 2;
    GLfloat halfHeight = (r.y2 - r.y1) / 2;

    // type이 0일 때: moving 값 1, 2, 3, 4 1위 2아래 3왼 4오
    if (r.type == 0) {
        // 상단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = 1;
        rectangles[rectCount].type = 1;
        rectCount++;

        // 상단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = 2;
        rectangles[rectCount].type = 1;
        rectCount++;

        // 하단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = 3;
        rectangles[rectCount].type = 1;
        rectCount++;

        // 하단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = 4;
        rectangles[rectCount].type = 1;
        rectCount++;
    }

    // type이 1일 때: moving 값 5, 6, 7, 8
    else if (r.type == 1) {
        // 상단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = 5;
        rectangles[rectCount].type = 2;
        rectCount++;

        // 상단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = 6;
        rectangles[rectCount].type = 2;
        rectCount++;

        // 하단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = 7;
        rectangles[rectCount].type = 2;
        rectCount++;

        // 하단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = 8;
        rectangles[rectCount].type = 2;
        rectCount++;
    }

    // type이 2일 때: moving 값 랜덤으로 1~4
    else if (r.type == 2) {
        int moving = rand() % 4 + 1;
        // 상단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = moving;
        rectangles[rectCount].type = 3;
        rectCount++;

        // 상단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1 + halfHeight;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y2;
        rectangles[rectCount].moving = moving;
        rectangles[rectCount].type = 3;
        rectCount++;

        // 하단 왼쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x1 + halfWidth;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = moving;
        rectangles[rectCount].type = 3;
        rectCount++;

        // 하단 오른쪽
        rectangles[rectCount].color[0] = r.color[0];
        rectangles[rectCount].color[1] = r.color[1];
        rectangles[rectCount].color[2] = r.color[2];
        rectangles[rectCount].x1 = r.x1 + halfWidth;
        rectangles[rectCount].y1 = r.y1;
        rectangles[rectCount].x2 = r.x2;
        rectangles[rectCount].y2 = r.y1 + halfHeight;
        rectangles[rectCount].moving = moving;
        rectangles[rectCount].type = 3;
        rectCount++;
    }

    // type이 3일 때: 8개로 쪼개기
    else if (r.type == 3) {
        GLfloat halfWidth = (r.x2 - r.x1) / 2;   // 2분할해서 사각형을 만듦
        GLfloat halfHeight = (r.y2 - r.y1) / 2;

        // 첫 번째 4개의 사각형: moving 값 1, 2, 3, 4
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                rectangles[rectCount].color[0] = r.color[0];
                rectangles[rectCount].color[1] = r.color[1];
                rectangles[rectCount].color[2] = r.color[2];
                rectangles[rectCount].x1 = r.x1 + i * halfWidth;
                rectangles[rectCount].y1 = r.y1 + j * halfHeight;
                rectangles[rectCount].x2 = rectangles[rectCount].x1 + halfWidth;
                rectangles[rectCount].y2 = rectangles[rectCount].y1 + halfHeight;

                rectangles[rectCount].moving = 1 +(i * 2 + j);  // moving 값 1, 2, 3, 4
                rectangles[rectCount].type = 3;
                rectCount++;
            }
        }

        // 나머지 4개의 사각형: moving 값 5, 6, 7, 8
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                rectangles[rectCount].color[0] = r.color[0];
                rectangles[rectCount].color[1] = r.color[1];
                rectangles[rectCount].color[2] = r.color[2];
                rectangles[rectCount].x1 = r.x1 + halfWidth / 2 + i * halfWidth;
                rectangles[rectCount].y1 = r.y1 + halfHeight / 2 + j * halfHeight;
                rectangles[rectCount].x2 = rectangles[rectCount].x1 + halfWidth;
                rectangles[rectCount].y2 = rectangles[rectCount].y1 + halfHeight;

                rectangles[rectCount].moving = 5 + (i * 2 + j);  // moving 값 5, 6, 7, 8
                rectangles[rectCount].type = 3;
                rectCount++;
            }
        }
    }

    // 원래 사각형 삭제
    for (int j = index; j < rectCount - 1; ++j) {
        rectangles[j] = rectangles[j + 1];
    }
    rectCount--;
}

// 두 사각형이 겹치는지 확인하는 함수
int isOverlap(Rect* r1, Rect* r2) {
    return !(r1->x2 < r2->x1 || r1->x1 > r2->x2 || r1->y2 < r2->y1 || r1->y1 > r2->y2);
}

void drawScene() {
    //--- 변경된 배경색 설정
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); // bgColor 값 사용
    glClear(GL_COLOR_BUFFER_BIT);

    // 그리기 부분 구현
    for (int i = 0; i < rectCount; ++i) {
        glColor3fv(rectangles[i].color);
        glRectf(rectangles[i].x1, rectangles[i].y1, rectangles[i].x2, rectangles[i].y2);
    }
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
    for (int i = 0; i < rectCount; ++i) {
        // 각 사각형의 크기 줄이기
        GLfloat width = rectangles[i].x2 - rectangles[i].x1;
        GLfloat height = rectangles[i].y2 - rectangles[i].y1;
        
        // 사각형 움직임 처리
        switch (rectangles[i].moving) {
        case 1: // 위로 이동
            rectangles[i].y1 += 0.01f;
            rectangles[i].y2 += 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }
            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 4: // 아래로 이동
            rectangles[i].y1 -= 0.01f;
            rectangles[i].y2 -= 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 3: // 왼쪽으로 이동
            rectangles[i].x1 -= 0.01f;
            rectangles[i].x2 -= 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 2: // 오른쪽으로 이동
            rectangles[i].x1 += 0.01f;
            rectangles[i].x2 += 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 5: // 좌상 (왼쪽 위)
            rectangles[i].x1 -= 0.01f; // 왼쪽 이동
            rectangles[i].x2 -= 0.01f;
            rectangles[i].y1 += 0.01f; // 위로 이동
            rectangles[i].y2 += 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 6: // 우상 (오른쪽 위)
            rectangles[i].x1 += 0.01f; // 오른쪽 이동
            rectangles[i].x2 += 0.01f;
            rectangles[i].y1 += 0.01f; // 위로 이동
            rectangles[i].y2 += 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 7: // 좌하 (왼쪽 아래)
            rectangles[i].x1 -= 0.01f; // 왼쪽 이동
            rectangles[i].x2 -= 0.01f;
            rectangles[i].y1 -= 0.01f; // 아래로 이동
            rectangles[i].y2 -= 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 8: // 우하 (오른쪽 아래)
            rectangles[i].x1 += 0.01f; // 오른쪽 이동
            rectangles[i].x2 += 0.01f;
            rectangles[i].y1 -= 0.01f; // 아래로 이동
            rectangles[i].y2 -= 0.01f;
            // 일정 크기 이하가 되면 사각형 삭제
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // 배열을 이동시키면 다시 현재 위치를 검사해야 함
                continue;
            }

            // 사각형 크기를 조금씩 줄임
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        }
    }

    glutPostRedisplay(); // 화면을 갱신
    glutTimerFunc(16, TimerFunction, 1); // 타이머 재설정
}

void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // 디버그 메시지 출력

    switch (key) {
        // r : 사각형들을 삭제.
    case 'r':
        rectCount = 5;
        for (int i = 0; i < rectCount; ++i) {
            randomizeColor(&rectangles[i]);
            float x = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
            float y = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
            rectangles[i].x1 = x - rectSize / 2;
            rectangles[i].y1 = y - rectSize / 2;
            rectangles[i].x2 = x + rectSize / 2;
            rectangles[i].y2 = y + rectSize / 2;
            rectangles[i].type = rand() % 4;
            rectangles[i].moving = 0;
        }
        break;
        // q : 프로그램을 종료한다.
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    }

    // 화면 갱신 요청
    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            for (int i = 0; i < rectCount; ++i) {
                if (isInsideRect(x, y, &rectangles[i])) {
                    clickRect = i;

                    offsetX = normalizedX - rectangles[i].x1;
                    offsetY = normalizedY - rectangles[i].y1;

                    // 사각형을 나누는 함수를 호출
                    splitRectangle(i);

                    break;
                }
            }
        }
        else if (state == GLUT_UP) {
            if (clickRect != -1) {
                clickRect = -1;  // 드래그 종료
            }
        }
    }
    glutPostRedisplay();
}

void Motion(int x, int y) {
    glutPostRedisplay(); // 화면 갱신 요청
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_X, WINDOW_Y);
    glutCreateWindow("Example1");

    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Unable to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("GLEW Initialized\n");

    // 사각형 데이터 초기화
    for (int i = 0; i < rectCount; ++i) {
        randomizeColor(&rectangles[i]);
        float x = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        float y = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        rectangles[i].x1 = x - rectSize / 2;
        rectangles[i].y1 = y - rectSize / 2;
        rectangles[i].x2 = x + rectSize / 2;
        rectangles[i].y2 = y + rectSize / 2;
        rectangles[i].type = rand() % 4;
        rectangles[i].moving = 0;
    }

    // 콜백 함수 설정
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    //glutMotionFunc(Motion);
    glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();

    return 0;
}