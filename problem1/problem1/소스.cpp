#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

GLfloat bgColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

int timer = 0;
int change = 0;

int main(int argc, char** argv)
{
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
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
    glutTimerFunc(1, TimerFunction, 1);
    glutMainLoop();

    return 0;
}

GLvoid drawScene()
{
    //--- 변경된 배경색 설정
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
    if (timer == 1) {
        change++;
        if (change == 10) {
            bgColor[0] = (GLfloat)rand() / RAND_MAX;
            bgColor[1] = (GLfloat)rand() / RAND_MAX;
            bgColor[2] = (GLfloat)rand() / RAND_MAX;
            glutPostRedisplay(); // 화면을 갱신
            change = 0;
        }
    }
    glutTimerFunc(16, TimerFunction, 1); // 애니메이션이 진행 중일 때만 타이머 재설정
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    std::cout << "Key pressed: " << key << std::endl; // 디버그 메시지 출력
    glutKeyboardFunc(Keyboard);

    switch (key) {
    case 'c':
        bgColor[0] = 0.0f; // R
        bgColor[1] = 1.0f; // G
        bgColor[2] = 1.0f; // B
        break;
    case 'm':
        bgColor[0] = 1.0f; // R
        bgColor[1] = 0.0f; // G
        bgColor[2] = 1.0f; // B
        break;
    case 'y':
        bgColor[0] = 1.0f; // R
        bgColor[1] = 1.0f; // G
        bgColor[2] = 0.0f; // B
        break;
    case 'a':
        bgColor[0] = (GLfloat)rand() / RAND_MAX;
        bgColor[1] = (GLfloat)rand() / RAND_MAX;
        bgColor[2] = (GLfloat)rand() / RAND_MAX;
        break;
    case 'w':
        bgColor[0] = 1.0f; // R
        bgColor[1] = 1.0f; // G
        bgColor[2] = 1.0f; // B
        break;
    case 'k':
        bgColor[0] = 0.0f; // R
        bgColor[1] = 0.0f; // G
        bgColor[2] = 0.0f; // B
        break;
    case 't':
        timer = 1;
        break;
    case 's':
        timer = 0;
        break;
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    }

    // 화면 갱신 요청
    glutPostRedisplay();
}
