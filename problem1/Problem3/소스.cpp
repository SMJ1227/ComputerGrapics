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

// ���� ���� ���� �Լ�
void randomizeColor(Rect* rect) {
    rect->color[0] = (GLfloat)rand() / RAND_MAX;
    rect->color[1] = (GLfloat)rand() / RAND_MAX;
    rect->color[2] = (GLfloat)rand() / RAND_MAX;
}

// �簢�� ����/�ܺ� Ŭ�� Ȯ�� �Լ�
int isInsideRect(int x, int y, Rect* rect) {
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y); // ������ ��ǥ���� OpenGL ��ǥ ��ȯ

    return (normalizedX >= rect->x1 && normalizedX <= rect->x2 &&
        normalizedY >= rect->y1 && normalizedY <= rect->y2);
}

// �� �簢���� ��ġ���� Ȯ���ϴ� �Լ�
int isOverlap(Rect* r1, Rect* r2) {
    return !(r1->x2 < r2->x1 || r1->x1 > r2->x2 || r1->y2 < r2->y1 || r1->y1 > r2->y2);
}

// �� �簢���� �����ϴ� �Լ�
void mergeRectangles(int index1, int index2) {
    if (index1 > index2) {
        // �׻� index1�� �� ���� �ε����� ����Ű���� ��
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }

    rectangles[index1].x1 = fmin(rectangles[index1].x1, rectangles[index2].x1);
    rectangles[index1].y1 = fmin(rectangles[index1].y1, rectangles[index2].y1);
    rectangles[index1].x2 = fmax(rectangles[index1].x2, rectangles[index2].x2);
    rectangles[index1].y2 = fmax(rectangles[index1].y2, rectangles[index2].y2);

    randomizeColor(&rectangles[index1]);  // ������ �����ϰ� ����

    // �� ��° �簢�� ����
    for (int j = index2; j < rectCount - 1; ++j) {
        rectangles[j] = rectangles[j + 1];
    }
    rectCount--;
}

// ���콺 ��ư�� ������ �� ȣ��Ǵ� �Լ�
void mouseButtonUp(int x, int y) {
    if (draggingRect != -1) {
        for (int i = 0; i < rectCount; ++i) {
            if (i != draggingRect && isOverlap(&rectangles[draggingRect], &rectangles[i])) {
                // �簢���� ��ġ�� �����ϰ� �� ��° �簢���� ����
                mergeRectangles(draggingRect, i);
                // ���� ���� �Ŀ��� ������ �ߴ��ؾ� ��
                break;
            }
        }
        draggingRect = -1;  // �巡�� ����
    }
}

// �ش� ������ ���콺 �Ǵ� ��� �̺�Ʈ���� ������ ȣ��


void drawScene() {
    //--- ����� ���� ����
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); // bgColor �� ���
    glClear(GL_COLOR_BUFFER_BIT);

    // �׸��� �κ� ����
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
    printf("Key pressed: %c\n", key); // ����� �޽��� ���

    switch (key) {
    case 'a':
        Rect newRect;
        randomizeColor(&newRect);

        newRect.x1 = ((GLfloat)rand() / RAND_MAX) * 1.5f - 1.0f;
        newRect.y1 = ((GLfloat)rand() / RAND_MAX) * 1.5f - 1.0f;
        newRect.x2 = newRect.x1 + rectSize;
        newRect.y2 = newRect.y1 + rectSize;

        // ���ο� �簢���� �߰��ϴ� ����
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

    // ȭ�� ���� ��û
    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
    printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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
            // �ٸ� �簢���� ��ġ���� Ȯ��
            for (int i = 0; i < rectCount; ++i) {
                if (i != draggingRect && isOverlap(&rectangles[draggingRect], &rectangles[i])) {
                    // �簢���� ��ġ�� ����
                    mergeRectangles(draggingRect, i);
                    // ��ġ�� ó���� �Ϸ�� �� for loop�� �������ɴϴ�.
                    break;
                }
            }
            // �巡�� ����
            draggingRect = -1;
        }
    }
}

void Motion(int x, int y) {
    if (draggingRect != -1) {
        GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
        GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);

        // ���� �巡�� ���� �簢���� ���� ���� ���� ���
        GLfloat originalWidth = rectangles[draggingRect].x2 - rectangles[draggingRect].x1;
        GLfloat originalHeight = rectangles[draggingRect].y2 - rectangles[draggingRect].y1;

        // �巡�� ���� �簢���� ��ġ�� ���콺�� ���� �����ϵ�, ������ ���� ���̸� ����
        rectangles[draggingRect].x1 = normalizedX - offsetX;
        rectangles[draggingRect].y1 = normalizedY - offsetY;
        rectangles[draggingRect].x2 = rectangles[draggingRect].x1 + originalWidth;
        rectangles[draggingRect].y2 = rectangles[draggingRect].y1 + originalHeight;

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
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Unable to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("GLEW Initialized\n");

    // �ݹ� �Լ� ����
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutMainLoop();

    return 0;
}