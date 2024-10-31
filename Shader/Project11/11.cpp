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

Colors colors[4] = {
	{0.0f, 0.0f, 1.0f},  // 파랑
	{0.0f, 0.0f, 0.0f},  // 검정
	{0.5f, 0.5f, 0.5f},  // 핑크
	{1.0f, 1.0f, 0.0f}   // 노랑
};

struct Matrix {
	GLfloat x, y;
	int shapeType;
};

Matrix matrix[4];

int numShape[4] = { 1, 1, 1, 1 };
int index[4] = { 1,1,1,1 };

GLfloat originpointX[4] = { 0.0f, 0.1f, 0.1f };
GLfloat originpointY[4] = { -0.1f , 0.0f, 0.1f };
GLfloat pointX[4] = { 0.0f, 0.1f , 0.1f};
GLfloat pointY[4] = { -0.1f , 0.0f, 0.1f};
GLfloat maxpointY[4] = { 0.1f , 0.1f, 0.2f };
bool increasing = true;

// 타이머 함수
void timerFunc(int value) {
	// Y 좌표가 증가하다가 일정 값 이상이면 감소
	if (increasing) {
		for (int i = 0; i < 3; i++) {
			pointX[i] += 0.02f;
			pointY[i] += 0.02f;  // Y 좌표 증가
			if (pointY[i] >= maxpointY[i]) {  // Y 좌표가 최대값에 도달했을 때
				pointX[i] = originpointX[i];
				pointY[i] = originpointY[i];
				matrix[i].shapeType = (matrix[i].shapeType - 1) % 4 + 2;
			}
		}
	}

	glutPostRedisplay();  // 화면 업데이트 요청
	glutTimerFunc(100, timerFunc, 0);  // 0.1초마다 타이머 재호출
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case 'a':
	case 'A':
		break;
	case 'b':
	case 'B':
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
		glutPostRedisplay();
}

void initializeTrianglePositions() {
	matrix[0].x = 0.5f; matrix[0].y = 0.5f;  // 1사분면
	matrix[0].shapeType = 2;
	matrix[1].x = -0.5f;  matrix[1].y = 0.5f;  // 2사분면
	matrix[1].shapeType = 3;
	matrix[2].x = -0.5f; matrix[2].y = -0.5f; // 3사분면
	matrix[2].shapeType = 4;
	matrix[3].x = 0.5f;  matrix[3].y = -0.5f; // 4사분면
	matrix[3].shapeType = 5;
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
	initializeTrianglePositions();

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
	glutTimerFunc(1000, timerFunc, 0);
	glutMainLoop();
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
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 셰이더 프로그램 활성화
	glUseProgram(shaderProgramID);  // 반드시 셰이더 프로그램 활성화

	// VAO 바인딩
	glBindVertexArray(vao);

	// 십자선 그리기
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
	glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), 0.0f, 0.0f);  // 십자선은 중앙에 그리기
	glDrawArrays(GL_LINES, 4, 2);  // 가로 선 그리기
	glDrawArrays(GL_LINES, 6, 2);  // 세로 선 그리기

	for (int i = 0; i < 4; i++) {
		// 각 사분면에 맞는 색 설정
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), colors[i].r, colors[i].g, colors[i].b);

		switch (matrix[i].shapeType) {
		case 2: // 선
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_LINES, 0, 2);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // 빨간색 점
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			
			break;
		case 3: // 삼각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // 빨간색 점
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x - (pointX[matrix[i].shapeType - 2]), matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			break;
		case 4: // 사각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // 빨간색 점
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x - 0.1f, matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			break;
		case 5: // 오각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 5); 
			break;	
		}
		
	}

	// VAO 언바인딩
	glBindVertexArray(0);

	// 화면에 출력
	glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	// 정점 데이터: 최대 4개의 점을 정의
	GLfloat vertices[] = {
		// x, y 좌표
		-0.1f, -0.1f, // 왼쪽 아래
		0.1f, -0.1f,  // 오른쪽 아래
		0.1f,  0.1f,  // 오른쪽 위
		-0.1f,  0.1f,  // 왼쪽 위
		-1.0f,  0.0f, // 가로 선 왼쪽 끝
		1.0f,  0.0f,  // 가로 선 오른쪽 끝
		0.0f,  1.0f,  // 세로 선 상단 끝
		0.0f, -1.0f,   // 세로 선 하단 끝
		0.1f,  0.0f,  // 첫 번째 점
		0.03f, 0.09f, // 두 번째 점
		-0.08f, 0.06f, // 세 번째 점
		-0.08f, -0.06f, // 네 번째 점
		0.03f, -0.09f  // 다섯 번째 점
	};

	// 인덱스 데이터
	GLuint indices[] = {
		0, 1, 2,  // 첫 번째 삼각형
		0, 2, 3,   // 두 번째 삼각형
		4, 5,     // 가로 선
		6, 7      // 세로 선
	};

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) 생성 및 데이터 전송
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 위치 속성 지정
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO 언바인딩
	glBindVertexArray(0);
}
