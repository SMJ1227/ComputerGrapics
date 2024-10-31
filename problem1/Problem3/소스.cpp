#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#define WINDOW_X 800
#define WINDOW_Y 600

GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat rectSize = 0.5f;

int draggingRect = -1;
GLfloat offsetX = 0.0f, offsetY = 0.0f;

typedef struct {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
} Rect;

int rectCount = 0;
Rect rectangles[10];

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

void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // 디버그 메시지 출력

    switch (key) {
    case 'a':
        Rect newRect;
        randomizeColor(&newRect);

        newRect.x1 = ((GLfloat)rand() / RAND_MAX) * 1.5f - 1.0f;
        newRect.y1 = ((GLfloat)rand() / RAND_MAX) * 1.5f - 1.0f;
        newRect.x2 = newRect.x1 + rectSize;
        newRect.y2 = newRect.y1 + rectSize;

        // 새로운 사각형을 추가하는 로직
        if (rectCount < 10) {
            rectangles[rectCount] = newRect;
            rectCount++;
        }
        else {
            for (int i = 0; i < 9; i++) {
                rectangles[i] = rectangles[i + 1];
            }
            rectangles[9] = newRect;
        }

        break;
    }

    // 화면 갱신 요청
    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
    printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (int i = 0; i < rectCount; ++i) {
            if (isInsideRect(x, y, &rectangles[i])) {
                draggingRect = i;

                GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
                GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);

                offsetX = normalizedX - rectangles[i].x1;
                offsetY = normalizedY - rectangles[i].y1;
                break;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (draggingRect != -1) {
            // 다른 사각형과 겹치는지 확인
            for (int i = 0; i < rectCount; ++i) {
                if (i != draggingRect && isOverlap(&rectangles[draggingRect], &rectangles[i])) {
                    // 사각형이 겹치면 병합
                    mergeRectangles(draggingRect, i);
                    // 겹치는 처리가 완료된 후 for loop를 빠져나옵니다.
                    break;
                }
            }
            // 드래그 종료
            draggingRect = -1;
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
    glutMotionFunc(Motion);
    glutMainLoop();

    return 0;
}