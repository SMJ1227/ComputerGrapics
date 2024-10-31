#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <cstdlib>
#include <ctime>

#define WINDOW_X 800
#define WINDOW_Y 600

GLfloat bgColor[4] = {0.0f, 0.0f, 1.0f, 1.0f}; // �ʱ� ����: �Ķ���
GLfloat rectSize = 0.5f;

struct Rect {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
};

Rect rectangles[4] = {
    {{1.0f, 1.0f, 1.0f}, -1.0f, -1.0f, 0.0f, 0.0f},  // ���� �Ʒ� �簢�� (���)
    {{1.0f, 0.0f, 0.0f}, 0.0f, -1.0f, 1.0f, 0.0f},   // ������ �Ʒ� �簢�� (������)
    {{0.0f, 1.0f, 0.0f}, -1.0f, 0.0f, 0.0f, 1.0f},   // ���� �� �簢�� (�ʷϻ�)
    {{0.0f, 0.0f, 1.0f}, 0.0f, 0.0f, 1.0f, 1.0f}     // ������ �� �簢�� (�Ķ���)
};

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

// ���� ���� ���� �Լ�
void randomizeColor(Rect& rect) {
    rect.color[0] = static_cast<GLfloat>(rand()) / RAND_MAX;
    rect.color[1] = static_cast<GLfloat>(rand()) / RAND_MAX;
    rect.color[2] = static_cast<GLfloat>(rand()) / RAND_MAX;
}

void randomizeBgColor() {
    bgColor[0] = static_cast<GLfloat>(rand()) / RAND_MAX;
    bgColor[1] = static_cast<GLfloat>(rand()) / RAND_MAX;
    bgColor[2] = static_cast<GLfloat>(rand()) / RAND_MAX;
}

// �簢�� ����/�ܺ� Ŭ�� Ȯ�� �Լ�
bool isInsideRect(int x, int y, Rect& rect) {
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y); // ������ ��ǥ���� OpenGL ��ǥ ��ȯ

    return (normalizedX >= rect.x1 && normalizedX <= rect.x2 &&
        normalizedY >= rect.y1 && normalizedY <= rect.y2);
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
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";
    
    // �ݹ� �Լ� ����
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMainLoop();

    return 0;
}

GLvoid drawScene() {
    //--- ����� ���� ����
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); // bgColor �� ���
    glClear(GL_COLOR_BUFFER_BIT);

    // �׸��� �κ� ����
    for (int i = 0; i < 4; ++i) {
        glColor3fv(rectangles[i].color);
        glRectf(rectangles[i].x1, rectangles[i].y1, rectangles[i].x2, rectangles[i].y2);
    }

    glutSwapBuffers();
}


GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    printf("Key pressed: %c\n", key); // ����� �޽��� ���
    // ȭ�� ���� ��û
    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
    printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        bool insideAnyRect = false;
        for (int i = 0; i < 4; i++) {
            if (isInsideRect(x, y, rectangles[i])) {
                randomizeColor(rectangles[i]);  // Ŭ���� �簢���� ���� ����
                insideAnyRect = true;
                break;
            }
        }
        if (!insideAnyRect) {
            randomizeBgColor();
        }
        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        GLfloat changeSize = 0.1f; // Ȯ���� ũ��
        bool insideAnyRect = false;
        for (int i = 0; i < 4; i++) {
            if (isInsideRect(x, y, rectangles[i])) {
                // �簢�� ���� Ŭ�� �� ũ�� ��� (�ּ� ũ�� ����)
                if (rectangles[i].x2 - rectangles[i].x1 > 0.2f && rectangles[i].y2 - rectangles[i].y1 > 0.2f) {
                    rectangles[i].x1 += changeSize;
                    rectangles[i].x2 -= changeSize;
                    rectangles[i].y1 += changeSize;
                    rectangles[i].y2 -= changeSize;
                }
                insideAnyRect = true;
                break;
            }
        }
        if (!insideAnyRect) {
            for (int i = 0; i < 4; ++i) {
                rectangles[i].x1 -= changeSize;
                rectangles[i].x2 += changeSize;
                rectangles[i].y1 -= changeSize;
                rectangles[i].y2 += changeSize;
            }
        }
        glutPostRedisplay();
    }

}