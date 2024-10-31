#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#define WINDOW_X 800
#define WINDOW_Y 600

GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat rectSize = 0.1f;
GLfloat mouseSize = 0.2f;

int drawEraser = 0;
int isEaten = 0;
GLfloat offsetX = 0.0f, offsetY = 0.0f;

typedef struct {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
} Rect;

int rectCount = 20;
Rect rectangles[20];
Rect eraser;

// 랜덤 색상 생성 함수
void randomizeColor(Rect* rect) {
    rect->color[0] = (GLfloat)rand() / RAND_MAX;
    rect->color[1] = (GLfloat)rand() / RAND_MAX;
    rect->color[2] = (GLfloat)rand() / RAND_MAX;
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
    if(drawEraser){
        glColor3fv(eraser.color);
        glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
    }
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
    glutPostRedisplay(); // 화면을 갱신
    glutTimerFunc(16, TimerFunction, 1); // 애니메이션이 진행 중일 때만 타이머 재설정
}


void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // 디버그 메시지 출력

    switch (key) {
        // r : 사각형들을 삭제.
    case 'r':
        rectCount = 20;
        for (int i = 0; i < rectCount; ++i) {
            randomizeColor(&rectangles[i]);
            float x = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
            float y = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
            rectangles[i].x1 = x - rectSize / 2;
            rectangles[i].y1 = y - rectSize / 2;
            rectangles[i].x2 = x + rectSize / 2;
            rectangles[i].y2 = y + rectSize / 2;
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
    printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            eraser.color[0] = 0.0f;
            eraser.color[1] = 0.0f;
            eraser.color[2] = 0.0f;

            eraser.x1 = normalizedX - mouseSize / 2;
            eraser.y1 = normalizedY - mouseSize / 2;
            eraser.x2 = eraser.x1 + mouseSize;
            eraser.y2 = eraser.y1 + mouseSize;

            drawEraser = 1;
        }
        else if (state == GLUT_UP) {
            drawEraser = 0;
            isEaten = 0;
        }
    }
    glutPostRedisplay();
}

void Motion(int x, int y) {
    if (drawEraser) {
        GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
        GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);
        GLfloat eraserSize;
        if (isEaten) {
            eraserSize = mouseSize + 0.1f;
        }
        else {
            eraserSize = mouseSize;
        }
        eraser.x1 = normalizedX - offsetX - eraserSize / 2;
        eraser.y1 = normalizedY - offsetY - eraserSize / 2;
        eraser.x2 = eraser.x1 + eraserSize;
        eraser.y2 = eraser.y1 + eraserSize;

        for (int i = 0; i < rectCount; ++i) {
            if (drawEraser && isOverlap(&eraser, &rectangles[i])) {
                // 겹쳐진 사각형을 삭제
                isEaten = 1;
                eraser.color[0] = rectangles[i].color[0];
                eraser.color[1] = rectangles[i].color[1];
                eraser.color[2] = rectangles[i].color[2];
                rectangles[i] = rectangles[rectCount - 1];
                rectCount--;
                break;
            }
        }
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

    // 사각형 데이터 초기화
    for (int i = 0; i < rectCount; ++i) {
        randomizeColor(&rectangles[i]);
        float x = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        float y = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        rectangles[i].x1 = x - rectSize / 2;
        rectangles[i].y1 = y - rectSize / 2;
        rectangles[i].x2 = x + rectSize / 2;
        rectangles[i].y2 = y + rectSize / 2;
    }

    // 콜백 함수 설정
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    //glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();

    return 0;
}