#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#define WINDOW_X 800
#define WINDOW_Y 600
#define MAX_SIZE 0.5f  // 사각형의 최대 크기
#define MIN_SIZE 0.1f  // 사각형의 최소 크기

GLfloat bgColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat rectSize = 0.5f;

int draggingRect = -1;
GLfloat offsetX = 0.0f, offsetY = 0.0f;

int isAnimating = 0;

typedef struct {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
    int goup, goright;
    int gobig;
} Rect;

int rectCount = 0;
Rect rectangles[5];
Rect origin[5];

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

// 두 사각형이 겹치는지 확인하는 함수
int isOverlap(Rect* r1, Rect* r2) {
    return !(r1->x2 < r2->x1 || r1->x1 > r2->x2 || r1->y2 < r2->y1 || r1->y1 > r2->y2);
}

// 두 사각형을 병합하는 함수
void mergeRectangles(int index1, int index2) {
    if (index1 > index2) {
        // 항상 index1이 더 작은 인덱스를 가리키도록 함
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }

    rectangles[index1].x1 = fmin(rectangles[index1].x1, rectangles[index2].x1);
    rectangles[index1].y1 = fmin(rectangles[index1].y1, rectangles[index2].y1);
    rectangles[index1].x2 = fmax(rectangles[index1].x2, rectangles[index2].x2);
    rectangles[index1].y2 = fmax(rectangles[index1].y2, rectangles[index2].y2);

    randomizeColor(&rectangles[index1]);  // 색상을 랜덤하게 변경

    // 두 번째 사각형 삭제
    for (int j = index2; j < rectCount - 1; ++j) {
        rectangles[j] = rectangles[j + 1];
    }
    rectCount--;
}

// 마우스 버튼을 놓았을 때 호출되는 함수
void mouseButtonUp(int x, int y) {
    if (draggingRect != -1) {
        for (int i = 0; i < rectCount; ++i) {
            if (i != draggingRect && isOverlap(&rectangles[draggingRect], &rectangles[i])) {
                // 사각형이 겹치면 병합하고 두 번째 사각형을 삭제
                mergeRectangles(draggingRect, i);
                // 병합 로직 후에는 루프를 중단해야 함
                break;
            }
        }
        draggingRect = -1;  // 드래그 종료
    }
}

// 해당 로직을 마우스 또는 모션 이벤트에서 적절히 호출
    
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
    if (isAnimating == 1) {
        for (int i = 0; i < rectCount; i++) {
            if (rectangles[i].goup == 1) {
                rectangles[i].y1 += 0.01f;
                rectangles[i].y2 += 0.01f;
            }
            else {
                rectangles[i].y1 -= 0.01f;
                rectangles[i].y2 -= 0.01f;
            }
            if (rectangles[i].goright == 1) {
                rectangles[i].x1 += 0.01f;
                rectangles[i].x2 += 0.01f;
            }
            else {
                rectangles[i].x1 -= 0.01f;
                rectangles[i].x2 -= 0.01f;
            }

            // 벽에 닿았을 때 방향 반전
            if (rectangles[i].y2 > 1.0f || rectangles[i].y1 < -1.0f) {
                rectangles[i].goup = !rectangles[i].goup;
            }
            if (rectangles[i].x2 > 1.0f || rectangles[i].x1 < -1.0f) {
                rectangles[i].goright = !rectangles[i].goright;
            }
        }
        glutPostRedisplay(); // 화면을 갱신
    }
    else if (isAnimating == 2) {
        for (int i = 0; i < rectCount; i++) {
            if (rectangles[i].goright == 1) {
                rectangles[i].x1 += 0.01f;
                rectangles[i].x2 += 0.01f;
            }
            else {
                rectangles[i].x1 -= 0.01f;
                rectangles[i].x2 -= 0.01f;
            }
            // 벽에 닿았을 때 방향 반전
            if (rectangles[i].y2 > 1.0f || rectangles[i].y1 < -1.0f) {
                rectangles[i].goup = !rectangles[i].goup;
            }
            if (rectangles[i].x2 > 1.0f || rectangles[i].x1 < -1.0f) {
                if (rectangles[i].goup == 1) {
                    rectangles[i].y1 += 0.2f;
                    rectangles[i].y2 += 0.2f;
                }
                else if (rectangles[i].goup == 0) {
                    rectangles[i].y1 -= 0.2f;
                    rectangles[i].y2 -= 0.2f;
                }
                rectangles[i].goright = !rectangles[i].goright;
            }
        }
        glutPostRedisplay(); // 화면을 갱신
    }
    else if (isAnimating == 3) {
        for (int i = 0; i < rectCount; i++) {
            GLfloat width = rectangles[i].x2 - rectangles[i].x1;
            GLfloat height = rectangles[i].y2 - rectangles[i].y1;

            if (rectangles[i].gobig == 1) {
                if (width < MAX_SIZE && height < MAX_SIZE) {
                    rectangles[i].x2 += 0.01f;
                    rectangles[i].y2 += 0.01f;
                }
                else {
                    rectangles[i].gobig = 0;  // 최대 크기에 도달하면 작아지기 시작
                }
            }
            else if (rectangles[i].gobig == 0) {
                if (width > MIN_SIZE && height > MIN_SIZE) {
                    rectangles[i].x2 -= 0.01f;
                    rectangles[i].y2 -= 0.01f;
                }
                else {
                    rectangles[i].gobig = 1;  // 최소 크기에 도달하면 커지기 시작
                }
            }
        }
        glutPostRedisplay(); // 화면을 갱신
    }
    glutTimerFunc(16, TimerFunction, 1); // 애니메이션이 진행 중일 때만 타이머 재설정
}


void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // 디버그 메시지 출력

    switch (key) {
        // 1: 위치 변화1 사각형들은 각각 대각선으로 이동하고 벽에 닿으면 튕겨 다른 방향으로 이동한다 / 멈춘다.
    case '1':
        if (isAnimating == 1) {
            isAnimating = 0;
            break;
        }
        for (int i = 0; i < rectCount; i++) {
            origin[i] = rectangles[i];
        }
        isAnimating = 1;
        break;
        // 2 : 위치 변화2 사각형들이 지그재그로 이동한다 / 멈춘다.
    case '2':
        if (isAnimating == 2) {
            isAnimating = 0;
            break;
        }
        for (int i = 0; i < rectCount; i++) {
            origin[i] = rectangles[i];
        }
        isAnimating = 2;
        break;
        // 3 : 크기 변화 사각형의 크기가 다양하게 변한다 / 변하지 않는다.
    case '3':
        if (isAnimating == 3) {
            isAnimating = 0;
            break;
        }
        for (int i = 0; i < rectCount; i++) {
            origin[i] = rectangles[i];
        }
        isAnimating = 3;
        break;
        // 4 : 색상 변화 사각형의 색상이 랜덤하게 변한다 / 변하지 않는다.
    case '4':
        for (int i = 0; i < rectCount; i++) {
            randomizeColor(&rectangles[i]);
        }
        break;
        // s : 모든 애니메이션이 멈춘다.
    case 's':
        isAnimating = 0;  // 애니메이션 멈춤
        break;
        // m : 원래 그린 위치로 사각형들이 이동한다.
    case 'm':
        for (int i = 0; i < rectCount; i++) {
            rectangles[i] = origin[i];
        }
        break;
        // r : 사각형들을 삭제하고 다시 마우스 입력을 받을 수 있다.
    case 'r':
        rectCount = 0;
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
    printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
        GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);

        Rect newRect;
        newRect.goup = 1;
        newRect.goright = 1;
        newRect.gobig = 0;
        randomizeColor(&newRect);

        newRect.x1 = normalizedX - rectSize / 2;
        newRect.y1 = normalizedY - rectSize / 2;
        newRect.x2 = newRect.x1 + rectSize;
        newRect.y2 = newRect.y1 + rectSize;

        // 새로운 사각형을 추가하는 로직
        if (rectCount < 5) {
            rectangles[rectCount] = newRect;
            rectCount++;
        }
        else {
            for (int i = 0; i < 4; i++) {
                rectangles[i] = rectangles[i + 1];
            }
            rectangles[4] = newRect;
        }
    }
}

void Motion(int x, int y) {
    if (draggingRect != -1) {
        GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
        GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);

        // 현재 드래그 중인 사각형의 원래 폭과 높이 계산
        GLfloat originalWidth = rectangles[draggingRect].x2 - rectangles[draggingRect].x1;
        GLfloat originalHeight = rectangles[draggingRect].y2 - rectangles[draggingRect].y1;

        // 드래그 중인 사각형의 위치를 마우스에 맞춰 조정하되, 원래의 폭과 높이를 유지
        rectangles[draggingRect].x1 = normalizedX - offsetX;
        rectangles[draggingRect].y1 = normalizedY - offsetY;
        rectangles[draggingRect].x2 = rectangles[draggingRect].x1 + originalWidth;
        rectangles[draggingRect].y2 = rectangles[draggingRect].y1 + originalHeight;

        glutPostRedisplay(); // 화면 갱신 요청
    }
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