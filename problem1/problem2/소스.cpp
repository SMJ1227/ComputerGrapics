#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <cstdlib>
#include <ctime>

#define WINDOW_X 800
#define WINDOW_Y 600

GLfloat bgColor[4] = {0.0f, 0.0f, 1.0f, 1.0f}; // 초기 배경색: 파란색
GLfloat rectSize = 0.5f;

struct Rect {
    GLfloat color[3];
    GLfloat x1, y1;
    GLfloat x2, y2;
};

Rect rectangles[4] = {
    {{1.0f, 1.0f, 1.0f}, -1.0f, -1.0f, 0.0f, 0.0f},  // 왼쪽 아래 사각형 (흰색)
    {{1.0f, 0.0f, 0.0f}, 0.0f, -1.0f, 1.0f, 0.0f},   // 오른쪽 아래 사각형 (빨간색)
    {{0.0f, 1.0f, 0.0f}, -1.0f, 0.0f, 0.0f, 1.0f},   // 왼쪽 위 사각형 (초록색)
    {{0.0f, 0.0f, 1.0f}, 0.0f, 0.0f, 1.0f, 1.0f}     // 오른쪽 위 사각형 (파란색)
};

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

// 랜덤 색상 생성 함수
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

// 사각형 내부/외부 클릭 확인 함수
bool isInsideRect(int x, int y, Rect& rect) {
    GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
    GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y); // 윈도우 좌표에서 OpenGL 좌표 변환

    return (normalizedX >= rect.x1 && normalizedX <= rect.x2 &&
        normalizedY >= rect.y1 && normalizedY <= rect.y2);
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
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";
    
    // 콜백 함수 설정
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMainLoop();

    return 0;
}

GLvoid drawScene() {
    //--- 변경된 배경색 설정
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); // bgColor 값 사용
    glClear(GL_COLOR_BUFFER_BIT);

    // 그리기 부분 구현
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
    printf("Key pressed: %c\n", key); // 디버그 메시지 출력
    // 화면 갱신 요청
    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
    printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        bool insideAnyRect = false;
        for (int i = 0; i < 4; i++) {
            if (isInsideRect(x, y, rectangles[i])) {
                randomizeColor(rectangles[i]);  // 클릭된 사각형의 색상 변경
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
        GLfloat changeSize = 0.1f; // 확대할 크기
        bool insideAnyRect = false;
        for (int i = 0; i < 4; i++) {
            if (isInsideRect(x, y, rectangles[i])) {
                // 사각형 내부 클릭 시 크기 축소 (최소 크기 제한)
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