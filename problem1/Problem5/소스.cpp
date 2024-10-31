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

// ���� ���� ���� �Լ�
void randomizeColor(Rect* rect) {
    rect->color[0] = (GLfloat)rand() / RAND_MAX;
    rect->color[1] = (GLfloat)rand() / RAND_MAX;
    rect->color[2] = (GLfloat)rand() / RAND_MAX;
}

// �� �簢���� ��ġ���� Ȯ���ϴ� �Լ�
int isOverlap(Rect* r1, Rect* r2) {
    return !(r1->x2 < r2->x1 || r1->x1 > r2->x2 || r1->y2 < r2->y1 || r1->y1 > r2->y2);
}

void drawScene() {
    //--- ����� ���� ����
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); // bgColor �� ���
    glClear(GL_COLOR_BUFFER_BIT);

    // �׸��� �κ� ����
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
    glutPostRedisplay(); // ȭ���� ����
    glutTimerFunc(16, TimerFunction, 1); // �ִϸ��̼��� ���� ���� ���� Ÿ�̸� �缳��
}


void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // ����� �޽��� ���

    switch (key) {
        // r : �簢������ ����.
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
        // q : ���α׷��� �����Ѵ�.
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    }

    // ȭ�� ���� ��û
    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
    printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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
                // ������ �簢���� ����
                isEaten = 1;
                eraser.color[0] = rectangles[i].color[0];
                eraser.color[1] = rectangles[i].color[1];
                eraser.color[2] = rectangles[i].color[2];
                rectangles[i] = rectangles[rectCount - 1];
                rectCount--;
                break;
            }
        }
        glutPostRedisplay(); // ȭ�� ���� ��û
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    //--- ������ �����ϱ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_X, WINDOW_Y);
    glutCreateWindow("Example1");

    //--- GLEW �ʱ�ȭ�ϱ�
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Unable to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("GLEW Initialized\n");

    // �簢�� ������ �ʱ�ȭ
    for (int i = 0; i < rectCount; ++i) {
        randomizeColor(&rectangles[i]);
        float x = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        float y = (float)rand() / RAND_MAX * 2.0f - 1.0f; // -1.0 ~ 1.0
        rectangles[i].x1 = x - rectSize / 2;
        rectangles[i].y1 = y - rectSize / 2;
        rectangles[i].x2 = x + rectSize / 2;
        rectangles[i].y2 = y + rectSize / 2;
    }

    // �ݹ� �Լ� ����
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    //glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();

    return 0;
}