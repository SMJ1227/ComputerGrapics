#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <time.h>
#include <math.h>

#define WINDOW_X 800
#define WINDOW_Y 600
#define M_PI 3.141592

//--- 아래 5개 함수는 사용자 정의 함수
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Initbuffer();
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) {	// Return NULL on failure 
		perror("ERROR: 쉐이더 파일을 열 수 없습니다.");
		return NULL;
	}
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

//--- 셰이더 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수	
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
GLuint vao, vbo;

//--- 전역 변수 선언
struct Colors {
	GLfloat r, g, b;
};

Colors bgColor = { 0.0f, 0.0f, 0.0f };

void randomizeColor() {
	bgColor.r = (GLfloat)rand() / RAND_MAX;
	bgColor.g = (GLfloat)rand() / RAND_MAX;
	bgColor.b = (GLfloat)rand() / RAND_MAX;
}

struct Points {
	GLfloat x, y;
};

Points points[5];

void randomizePoint(int i) {
	points[i].x = ((GLfloat)rand() / RAND_MAX) * 2.0f - 1.0f;
	points[i].y = ((GLfloat)rand() / RAND_MAX) * 2.0f - 1.0f;
}

GLint mousePosLocation;
GLfloat mouseX = 0.0f, mouseY = 0.0f;

GLfloat rotationAngleX = 0.0f;
int currentPointIndex = 0;
const int numPoints = 190;
int drawSpiral = 0;
int drawMode = 0;

GLfloat vertices[200 * 2]; // x, y 좌표로 구성된 정점 배열

void Timer(int value) {
	if (currentPointIndex < numPoints) {
		currentPointIndex++;
	}

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 0);
}
void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case 'p':
		drawMode = 0;
		break;
	case 'l':
		drawMode = 1;
		break;
	case '1':
		drawSpiral = 1;
		randomizePoint(0);
		currentPointIndex = 0;
		break;
	case '2':
		drawSpiral = 2;
		for (int i = 0; i < 2; i++) {
			randomizePoint(i);
		}
		currentPointIndex = 0;
		break;
	case '3':
		drawSpiral = 3;
		for (int i = 0; i < 3; i++) {
			randomizePoint(i);
		}
		currentPointIndex = 0;
		break;
	case '4':
		drawSpiral = 4;
		for (int i = 0; i < 4; i++) {
			randomizePoint(i);
		}
		currentPointIndex = 0;
		break;
	case '5':
		drawSpiral = 5;
		for (int i = 0; i < 5; i++) {
			randomizePoint(i);
		}
		currentPointIndex = 0;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// 클릭한 위치에 나선형 그리기
		mouseX = (2.0f * x / WINDOW_X) - 1.0f;
		mouseY = 1.0f - (2.0f * y / WINDOW_Y);
		points[0].x = mouseX;
		points[0].y = mouseY;
		currentPointIndex = 0;
		randomizeColor();
		if (drawSpiral < 1) {
			drawSpiral = 1;
		}
		glutPostRedisplay(); // 화면을 다시 그리기 위해 호출
	}
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram();
	Initbuffer();

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard); // 키 입력 콜백
	glutMouseFunc(Mouse); // 마우스 위치 콜백
	glutTimerFunc(0, Timer, 0);
	glutMainLoop();
}

void checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER-COMPILATION-ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::PROGRAM-LINKING-ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader 파일을 불러오지 못했습니다." << std::endl;
	}
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//checkCompileErrors(vertexShader, "VERTEX");
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	if (!fragmentSource) {
		std::cerr << "ERROR: fragment shader 파일을 불러오지 못했습니다." << std::endl;
	}
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 세이더 프로그램 만들고 세이더 객체 링크하기
void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기

	//-- shader Program
	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);


	//--- 세이더 삭제하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);

	// 마우스 위치 uniform 변수 위치 얻기
	mousePosLocation = glGetUniformLocation(shaderProgramID, "mousePos");
}

GLvoid drawScene() {
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);  // 셰이더 프로그램 활성화

	for (int i = 0; i < drawSpiral; i++) {
		if (drawMode == 0) {
			// 첫 번째 나선 (커지는 방향)
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), points[i].x, points[i].y);
			glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), rotationAngleX);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 1.0f, 1.0f);
			glBindVertexArray(vao);
			glDrawArrays(GL_POINTS, 0, currentPointIndex);
			
			// 두 번째 나선 (작아지는 방향)
			if (currentPointIndex >= currentPointIndex / 2) {
				glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), points[i].x, points[i].y);
				glDrawArrays(GL_POINTS, 95, currentPointIndex - 95);
			}
		}
		else if (drawMode == 1) {
			for (int i = 0; i < drawSpiral; i++) {
				glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), points[i].x, points[i].y);
				glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), rotationAngleX);
				glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 1.0f, 1.0f);
				glBindVertexArray(vao);
				glDrawArrays(GL_LINE_STRIP, 0, currentPointIndex); // 점을 선으로 연결하여 그리기
			}
		}
		
	}

	glBindVertexArray(0);
	glutSwapBuffers();  // 화면 출력
}


//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	const int numPoints = 200;
	GLfloat angleIncrement = 0.2f;
	GLfloat radius = 0.02f;

	// 첫 번째 나선 (첫 95개의 점)
	for (int i = 0; i < 95; i++) {
		GLfloat angle = i * angleIncrement;
		vertices[i * 2] = radius * cos(angle);   // x 좌표
		vertices[i * 2 + 1] = radius * sin(angle); // y 좌표
		radius += 0.003f; // 나선의 반지름 증가
	}

	// 첫 번째 나선의 95번째 점 좌표
	GLfloat lastX = vertices[94 * 2];   // 95번째 점의 x 좌표
	GLfloat lastY = vertices[94 * 2 + 1]; // 95번째 점의 y 좌표

	// 두 번째 나선의 원점은 첫 번째 나선의 원점과 95번째 점 사이의 거리만큼 오른쪽으로 이동
	GLfloat originX = 2 * lastX; // 95번째 점에서 오른쪽으로 이동한 새로운 원점 x 좌표
	GLfloat originY = lastY;     // 새로운 원점의 y 좌표는 동일하게 유지

	// 두 번째 나선 (큰 반지름에서 작은 반지름으로 원점에 수렴)
	radius = 0.02f + (94 * 0.003f); // 두 번째 나선은 큰 반지름에서 시작
	for (int i = 95; i < numPoints; i++) {
		GLfloat angle = (i - 95) * angleIncrement;
		vertices[i * 2] = originX - (radius * cos(-angle));   // 새 원점을 기준으로 x 좌표 계산 (작아지는 방향)
		vertices[i * 2 + 1] = originY - (radius * sin(-angle)); // 새 원점을 기준으로 y 좌표 계산
		radius -= 0.003f; // 나선의 반지름 감소
	}

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 위치 속성 지정
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO 언바인딩
	glBindVertexArray(0);
}
