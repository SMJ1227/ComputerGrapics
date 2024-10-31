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

Colors colors[4] = {
	{0.0f, 0.0f, 1.0f},  // 파랑
	{0.0f, 0.0f, 0.0f},  // 검정
	{1.0f, 0.0f, 0.0f},  // 빨강
	{1.0f, 1.0f, 0.0f}   // 노랑
};

void randomizeColor(int ground) {
	colors[ground].r = (GLfloat)rand() / RAND_MAX;
	colors[ground].g = (GLfloat)rand() / RAND_MAX;
	colors[ground].b = (GLfloat)rand() / RAND_MAX;
}

struct Matrix {
	GLfloat x, y;
};

const int MAX_SHAPES = 3;
Matrix matrix[4][MAX_SHAPES];

int numShape[4] = { 1, 1, 1, 1 };
int index[4] = { 1,1,1,1 };

GLint mousePosLocation;
GLfloat mouseX = 0.0f, mouseY = 0.0f;

int moveMode = 0;
GLfloat speed = 0.01f;
GLfloat angle[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLint directionX[4] = { 1, 1, 1, 1 };
GLint directionY[4] = { 1, 1, 1, 1 };

GLfloat maxStep = 10.0f;
GLfloat spiralStep = 10.0f; // 현재 남은 이동 거리
int spiralDirection = 0;   // 현재 이동 방향 (0: 오른쪽, 1: 아래쪽, 2: 왼쪽, 3: 위쪽)
GLfloat spiralShrink = 2.0f; // 스파이럴 축소 정도

int checkGround(GLfloat normalizedX, GLfloat normalizedY) {
	if (normalizedX > 0 && normalizedY > 0) {
		return 0;
	}
	else if (normalizedX < 0 && normalizedY > 0) {
		return 1;
	}
	else if (normalizedX < 0 && normalizedY < 0) {
		return 2;
	}
	else if (normalizedX > 0 && normalizedY < 0) {
		return 3;
	}
}

void Timer(int value) {
	switch (moveMode)
	{
	case 1:
	{
		for (int i = 0; i < 4; i++) {	
			// 삼각형 위치 업데이트
			matrix[i][0].x += directionX[i] * speed;
			matrix[i][0].y += directionY[i] * speed;
			
			// 벽 충돌 처리 - X축
			if (matrix[i][0].x >= 1.0f) {
				directionX[i] = -1; // X 방향 반전
				matrix[i][0].x = 1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}
			else if (matrix[i][0].x <= -1.0f) {
				directionX[i] = 1; // X 방향 반전
				matrix[i][0].x = -1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}

			// 벽 충돌 처리 - Y축
			if (matrix[i][0].y >= 1.0f) {
				directionY[i] = -1; // Y 방향 반전
				matrix[i][0].y = 1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}
			else if (matrix[i][0].y <= -1.0f) {
				directionY[i] = 1; // Y 방향 반전
				matrix[i][0].y = -1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}

			// 각도 설정: 이동 방향에 따라 설정
			if (directionX[i] == 1 && directionY[i] == 1) {
				angle[i] = 45.0f;  // 오른쪽 위 방향
			}
			else if (directionX[i] == -1 && directionY[i] == 1) {
				angle[i] = 135.0f; // 왼쪽 위 방향
			}
			else if (directionX[i] == -1 && directionY[i] == -1) {
				angle[i] = 225.0f; // 왼쪽 아래 방향
			}
			else if (directionX[i] == 1 && directionY[i] == -1) {
				angle[i] = 315.0f; // 오른쪽 아래 방향
			}
		}
	}
	break;

	case 2:
	{
		for (int i = 0; i < 4; i++) {
			// 삼각형 위치 업데이트
			matrix[i][0].x += directionX[i] * speed;

			// 벽 충돌 처리
			if (matrix[i][0].x >= 1.0f) {
				directionX[i] = -1;
				matrix[i][0].x = 1.0f;
				angle[i] = 270.0f;
				if (directionY[i] == 1) matrix[i][0].y += 0.2f;
				else matrix[i][0].y -= 0.2f;
			}
			else if (matrix[i][0].x <= -1.0f) {
				directionX[i] = 1;
				matrix[i][0].x = -1.0f;
				angle[i] = 90.0f;
				if (directionY[i] == 1) matrix[i][0].y += 0.2f;
				else matrix[i][0].y -= 0.2f;
			}

			// 화면 아래쪽에 도달하면 방향을 위로 전환
			if (matrix[i][0].y <= -1.0f) {
				directionY[i] = 1; // 위쪽으로 이동
			}
			// 화면 위쪽에 도달하면 방향을 아래로 전환
			else if (matrix[i][0].y >= 1.0f) {
				directionY[i] = -1; // 아래쪽으로 이동
			}
		}
	}
	break;
	case 3: 
	{
		if (spiralStep <= -0.2f) break;
		for (int i = 0; i < 4; i++) {
			switch (spiralDirection) {
			case 0: // 오른쪽
				matrix[i][0].x += speed;
				angle[i] = 90.0f;
				break;
			case 1: // 아래쪽
				matrix[i][0].y -= speed;
				angle[i] = 180.0f;
				break;
			case 2: // 왼쪽
				matrix[i][0].x -= speed;
				angle[i] = 270.0f;
				break;
			case 3: // 위쪽
				matrix[i][0].y += speed;
				angle[i] = 0.0f;
				break;
			}
		}
		spiralStep -= 0.1f;
		if (spiralStep <= 0.0f) {
			spiralDirection = (spiralDirection + 1) % 4; // 방향 순환
			if (spiralDirection == 0 || spiralDirection == 2) {
				maxStep -= spiralShrink;
				spiralStep = maxStep;
			}
			else {
				spiralStep = maxStep;
			}
		}
	}
		break;
	case 4:
	{
		static GLfloat radius = 0.1f; // 초기 반지름 설정
		static GLfloat theta = 0.0f; // 각도 (라디안)

		for (int i = 0; i < 4; i++) {
			theta += speed;

			matrix[i][0].x = radius * cos(theta);
			matrix[i][0].y = radius * sin(theta);
			angle[i] = theta * 180.0f / M_PI; // 각도를 설정

			radius += 0.0005f;

			// 화면 범위를 벗어나면 반지름 초기화
			if (radius > 1.5f) {
				radius = 0.1f;
				theta = 0.0f;
			}
		}
	}
	break;

	}

	// 화면 업데이트
	glutPostRedisplay();
	// 타이머 재등록 (다시 호출)
	glutTimerFunc(16, Timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case '1':
		angle[0] = 45.0f;
		angle[1] = 45.0f;
		angle[2] = 45.0f;
		angle[3] = 45.0f;
		moveMode = 1;
		break;
	case '2':
		angle[0] = 90.0f;
		angle[1] = 90.0f;
		angle[2] = 90.0f;
		angle[3] = 90.0f;
		moveMode = 2;
		break;
	case '3':
		matrix[0][0].x = 0.0f; matrix[0][0].y = 1.0f;  // 1사분면
		matrix[1][0].x = -1.0f;  matrix[1][0].y = 1.0f;  // 2사분면
		matrix[2][0].x = -1.0f; matrix[2][0].y = 0.0f; // 3사분면
		matrix[3][0].x = 0.0f;  matrix[3][0].y = 0.0f; // 4사분면
		maxStep = 10.0f;
		spiralStep = 10.0f;
		spiralDirection = 0;
		spiralShrink = 2.0f;
		moveMode = 3;
		break;
	case '4':
		matrix[0][0].x = 0.5f; matrix[0][0].y = 0.5f;  // 1사분면
		matrix[1][0].x = -0.5f;  matrix[1][0].y = 0.5f;  // 2사분면
		matrix[2][0].x = -0.5f; matrix[2][0].y = -0.5f; // 3사분면
		matrix[3][0].x = 0.5f;  matrix[3][0].y = -0.5f; // 4사분면
		moveMode = 4;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}

void Mouse(int button, int state, int x, int y) {
	//printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
	GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
	GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);
	int ground = checkGround(normalizedX, normalizedY);
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			matrix[ground][0].x = normalizedX;
			matrix[ground][0].y = normalizedY;
			randomizeColor(ground);
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			if (numShape[ground] < 3) {
				matrix[ground][index[ground]].x = normalizedX;
				matrix[ground][index[ground]].y = normalizedY;
				numShape[ground]++;
				index[ground] = index[ground] % 2 + 1;
			}
			else {
				matrix[ground][index[ground]].x = normalizedX;
				matrix[ground][index[ground]].y = normalizedY;
				index[ground] = index[ground] % 2 + 1;
			}
		}
	}
	glutPostRedisplay();
}

void initializeTrianglePositions() {
	matrix[0][0].x = 0.5f; matrix[0][0].y = 0.5f;  // 1사분면
	matrix[1][0].x = -0.5f;  matrix[1][0].y = 0.5f;  // 2사분면
	matrix[2][0].x = -0.5f; matrix[2][0].y = -0.5f; // 3사분면
	matrix[3][0].x = 0.5f;  matrix[3][0].y = -0.5f; // 4사분면
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

//--- 출력 콜백 함수
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
	/*
	// 십자선 그리기
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
	glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), 0.0f, 0.0f);  // 십자선은 중앙에 그리기
	glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), 0.0f);
	glDrawArrays(GL_LINES, 3, 2);  // 가로 선 그리기
	glDrawArrays(GL_LINES, 5, 2);  // 세로 선 그리기
	*/

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < numShape[i]; j++) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), colors[i].r, colors[i].g, colors[i].b);
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i][j].x, matrix[i][j].y);
			glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), angle[i] * M_PI / 180.0f); // 각도를 라디안으로 변환

			// 삼각형 그리기
			glDrawArrays(GL_TRIANGLES, 0, 3);
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
		0.0f, 0.1f,   // 가운데 위
		-0.1f, -0.1f, // 왼쪽 아래
		0.1f, -0.1f,  // 오른쪽 아래
		-1.0f,  0.0f, // 가로 선 왼쪽 끝
		1.0f,  0.0f,  // 가로 선 오른쪽 끝
		0.0f,  1.0f,  // 세로 선 상단 끝
		0.0f, -1.0f   // 세로 선 하단 끝

	};

	// 인덱스 데이터
	GLuint indices[] = {
		0, 1, 2,	// 삼각형 그리기
		5, 6,     // 가로 선
		7, 8      // 세로 선
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


