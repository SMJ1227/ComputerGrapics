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

void splitRectangle(int index) {
    Rect r = rectangles[index];

    GLfloat halfWidth = (r.x2 - r.x1) / 2;
    GLfloat halfHeight = (r.y2 - r.y1) / 2;

    // type�� 0�� ��: moving �� 1, 2, 3, 4 1�� 2�Ʒ� 3�� 4��
    if (r.type == 0) {
        // ��� ����
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

        // ��� ������
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

        // �ϴ� ����
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

        // �ϴ� ������
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

    // type�� 1�� ��: moving �� 5, 6, 7, 8
    else if (r.type == 1) {
        // ��� ����
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

        // ��� ������
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

        // �ϴ� ����
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

        // �ϴ� ������
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

    // type�� 2�� ��: moving �� �������� 1~4
    else if (r.type == 2) {
        int moving = rand() % 4 + 1;
        // ��� ����
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

        // ��� ������
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

        // �ϴ� ����
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

        // �ϴ� ������
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

    // type�� 3�� ��: 8���� �ɰ���
    else if (r.type == 3) {
        GLfloat halfWidth = (r.x2 - r.x1) / 2;   // 2�����ؼ� �簢���� ����
        GLfloat halfHeight = (r.y2 - r.y1) / 2;

        // ù ��° 4���� �簢��: moving �� 1, 2, 3, 4
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                rectangles[rectCount].color[0] = r.color[0];
                rectangles[rectCount].color[1] = r.color[1];
                rectangles[rectCount].color[2] = r.color[2];
                rectangles[rectCount].x1 = r.x1 + i * halfWidth;
                rectangles[rectCount].y1 = r.y1 + j * halfHeight;
                rectangles[rectCount].x2 = rectangles[rectCount].x1 + halfWidth;
                rectangles[rectCount].y2 = rectangles[rectCount].y1 + halfHeight;

                rectangles[rectCount].moving = 1 +(i * 2 + j);  // moving �� 1, 2, 3, 4
                rectangles[rectCount].type = 3;
                rectCount++;
            }
        }

        // ������ 4���� �簢��: moving �� 5, 6, 7, 8
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                rectangles[rectCount].color[0] = r.color[0];
                rectangles[rectCount].color[1] = r.color[1];
                rectangles[rectCount].color[2] = r.color[2];
                rectangles[rectCount].x1 = r.x1 + halfWidth / 2 + i * halfWidth;
                rectangles[rectCount].y1 = r.y1 + halfHeight / 2 + j * halfHeight;
                rectangles[rectCount].x2 = rectangles[rectCount].x1 + halfWidth;
                rectangles[rectCount].y2 = rectangles[rectCount].y1 + halfHeight;

                rectangles[rectCount].moving = 5 + (i * 2 + j);  // moving �� 5, 6, 7, 8
                rectangles[rectCount].type = 3;
                rectCount++;
            }
        }
    }

    // ���� �簢�� ����
    for (int j = index; j < rectCount - 1; ++j) {
        rectangles[j] = rectangles[j + 1];
    }
    rectCount--;
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
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
    for (int i = 0; i < rectCount; ++i) {
        // �� �簢���� ũ�� ���̱�
        GLfloat width = rectangles[i].x2 - rectangles[i].x1;
        GLfloat height = rectangles[i].y2 - rectangles[i].y1;
        
        // �簢�� ������ ó��
        switch (rectangles[i].moving) {
        case 1: // ���� �̵�
            rectangles[i].y1 += 0.01f;
            rectangles[i].y2 += 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }
            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 4: // �Ʒ��� �̵�
            rectangles[i].y1 -= 0.01f;
            rectangles[i].y2 -= 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 3: // �������� �̵�
            rectangles[i].x1 -= 0.01f;
            rectangles[i].x2 -= 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 2: // ���������� �̵�
            rectangles[i].x1 += 0.01f;
            rectangles[i].x2 += 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 5: // �»� (���� ��)
            rectangles[i].x1 -= 0.01f; // ���� �̵�
            rectangles[i].x2 -= 0.01f;
            rectangles[i].y1 += 0.01f; // ���� �̵�
            rectangles[i].y2 += 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 6: // ��� (������ ��)
            rectangles[i].x1 += 0.01f; // ������ �̵�
            rectangles[i].x2 += 0.01f;
            rectangles[i].y1 += 0.01f; // ���� �̵�
            rectangles[i].y2 += 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 7: // ���� (���� �Ʒ�)
            rectangles[i].x1 -= 0.01f; // ���� �̵�
            rectangles[i].x2 -= 0.01f;
            rectangles[i].y1 -= 0.01f; // �Ʒ��� �̵�
            rectangles[i].y2 -= 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        case 8: // ���� (������ �Ʒ�)
            rectangles[i].x1 += 0.01f; // ������ �̵�
            rectangles[i].x2 += 0.01f;
            rectangles[i].y1 -= 0.01f; // �Ʒ��� �̵�
            rectangles[i].y2 -= 0.01f;
            // ���� ũ�� ���ϰ� �Ǹ� �簢�� ����
            if (width < 0.01f || height < 0.01f) {
                for (int j = i; j < rectCount - 1; ++j) {
                    rectangles[j] = rectangles[j + 1];
                }
                rectCount--;
                i--; // �迭�� �̵���Ű�� �ٽ� ���� ��ġ�� �˻��ؾ� ��
                continue;
            }

            // �簢�� ũ�⸦ ���ݾ� ����
            rectangles[i].x1 += 0.005f;
            rectangles[i].y1 += 0.005f;
            rectangles[i].x2 -= 0.005f;
            rectangles[i].y2 -= 0.005f;
            break;
        }
    }

    glutPostRedisplay(); // ȭ���� ����
    glutTimerFunc(16, TimerFunction, 1); // Ÿ�̸� �缳��
}

void Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // ����� �޽��� ���

    switch (key) {
        // r : �簢������ ����.
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
        // q : ���α׷��� �����Ѵ�.
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    }

    // ȭ�� ���� ��û
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

                    // �簢���� ������ �Լ��� ȣ��
                    splitRectangle(i);

                    break;
                }
            }
        }
        else if (state == GLUT_UP) {
            if (clickRect != -1) {
                clickRect = -1;  // �巡�� ����
            }
        }
    }
    glutPostRedisplay();
}

void Motion(int x, int y) {
    glutPostRedisplay(); // ȭ�� ���� ��û
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
        rectangles[i].type = rand() % 4;
        rectangles[i].moving = 0;
    }

    // �ݹ� �Լ� ����
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    //glutMotionFunc(Motion);
    glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();

    return 0;
}