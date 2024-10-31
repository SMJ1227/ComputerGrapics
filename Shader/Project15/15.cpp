#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <time.h>
#include <math.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

#define WINDOW_X 800
#define WINDOW_Y 600

using namespace glm;

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
GLfloat mouseX = 0.0f, mouseY = 0.0f;

mat4 view;
mat4 projection;

GLUquadricObj* qobj;

vec3 cameraPos = vec3(0.0f, 1.0f, 3.0f);		//--- 카메라 위치
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- 카메라 바라보는 방향
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- 카메라 위쪽 방향

int shape = 0;
int depte = 0;
int fill = 0;

GLfloat rotationAngleX = 0.0f;
int timerX = 0;
GLfloat rotationAngle = 0.0f;
int timerY = 0;
int jOrg = 0;
int only = 0;

void setupCamera() {
	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
}

// 타이머 함수
void timerFunc(int value) {
	if (timerX == 1) {
		rotationAngleX += radians(1.0f);
	}
	else if (timerX == -1) {
		rotationAngleX -= radians(1.0f);
	}
	if (timerY == 1) {
		rotationAngle += radians(1.0f);
	}
	else if (timerY == -1) {
		rotationAngle -= radians(1.0f);
	}
	glutPostRedisplay();  // 화면 업데이트 요청
	glutTimerFunc(100, timerFunc, 0);  // 0.1초마다 타이머 재호출
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case '1':
		only = 1;
		break;
	case '2':
		only = 2;
		break;
	case '3':
		only = 0;
		break;
	case 'i':
		cameraPos += vec3(0.0f, 0.5f, 0.0f);
		//cameraDirection += vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'j':
		cameraPos -= vec3(0.5f, 0.0f, 0.0f);
		//cameraDirection -= vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'k':
		cameraPos -= vec3(0.0f, 0.5f, 0.0f);
		//cameraDirection -= vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'l':
		cameraPos += vec3(0.5f, 0.0f, 0.0f);
		//cameraDirection += vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'c':
		shape = 0;
		break;
	case 't':
		shape = 1;
		break;
	case 'x':
		timerX = 1;
		break;
	case 'X':
		timerX = -1;
		break;
	case 'y':
		timerY = 1;
		break;
	case 'Y':
		timerY = -1;
		break;
	case 'r':
		jOrg = 1;
		break;
	case 's':
		jOrg = 0;
		rotationAngleX = 0.0f;
		rotationAngle = 0.0f;
		timerX = 0;
		timerY = 0;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	mouseX = (2.0f * x / WINDOW_X) - 1.0f;
	mouseY = 1.0f - (2.0f * y / WINDOW_Y);
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {

		}
		else if (state == GLUT_UP) {

		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	glutPostRedisplay(); // 화면 갱신 요청
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(NULL);
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram();
	Initbuffer();

	glEnable(GL_DEPTH_TEST);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard); // 키 입력 콜백
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
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
GLvoid drawScene() {
	//--- 배경색 설정 및 버퍼 클리어
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 셰이더 프로그램 활성화
	glUseProgram(shaderProgramID);
	// VAO 바인딩
	glBindVertexArray(vao);
	// 그리기
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 1.0f, 0.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	mat4 model = mat4(1.0f);
	if (jOrg == 0) {
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		if (only == 1 || only == 0) {
			if (timerX == 1 || timerX == -1)
				model = rotate(model, rotationAngleX, vec3(1.0f, 0.0f, 0.0f));  // X축을 기준으로 회전
			if (timerY == 1 || timerY == -1)
				model = rotate(model, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (only == 1 || only == 0) {
			if (timerX == 1 || timerX == -1)
				model = rotate(model, rotationAngleX, vec3(1.0f, 0.0f, 0.0f));  // X축을 기준으로 회전
			if (timerY == 1 || timerY == -1)
				model = rotate(model, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
		}
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	if (shape == 0) {	// 육면체
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else if (shape == 1) {  // 사각뿔
		GLvoid* offset;
		offset = (GLvoid*)(18 * sizeof(GLuint));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset);
		offset = (GLvoid*)(36 * sizeof(GLuint));
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, offset);
	}

	mat4 model2 = mat4(1.0f);
	if (jOrg == 0) {
		model2 = glm::translate(model2, glm::vec3(-1.0f, 0.0f, 0.0f));
		if (only == 2 || only == 0) {
			if (timerX == 1 || timerX == -1)
				model2 = rotate(model2, rotationAngleX, vec3(1.0f, 0.0f, 0.0f));  // X축을 기준으로 회전
			if (timerY == 1 || timerY == -1)
				model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (only == 2 || only == 0) {
			if (timerX == 1 || timerX == -1)
				model2 = rotate(model2, rotationAngleX, vec3(1.0f, 0.0f, 0.0f));  // X축을 기준으로 회전
			if (timerY == 1 || timerY == -1)
				model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
		}
		model2 = glm::translate(model2, glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	if (shape == 0) {
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
		qobj = gluNewQuadric(); // 객체 생성하기
		gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
		gluSphere(qobj, 0.5, 50, 50); // 객체 만들기
	}
	else if (shape == 1) {
		model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, -0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
		qobj = gluNewQuadric(); // 객체 생성하기
		gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능	
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
		gluCylinder(qobj, 0.5, 0.5, 1.0, 20, 8);
	}

	// 라인 축
	mat4 xyzModel = mat4(1.0f); // 회전 없음
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(xyzModel));

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(48 * sizeof(GLuint))); // X축 그리기
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(50 * sizeof(GLuint))); // Y축 그리기
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(52 * sizeof(GLuint))); // Z축 그리기

	// VAO 언바인딩
	glBindVertexArray(0);

	//--- 버퍼 스왑
	glutSwapBuffers();
}


//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	setupCamera();
	GLfloat vertices[] = {
		// 좌표             // 색상 (R, G, B)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // 뒤쪽 아래 왼쪽 (빨강)
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 뒤쪽 아래 오른쪽 (초록)
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 뒤쪽 위 오른쪽 (파랑)
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // 뒤쪽 위 왼쪽 (노랑)
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, // 앞쪽 아래 왼쪽 (핑크)
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 앞쪽 아래 오른쪽 (하늘색)
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, // 앞쪽 위 오른쪽 (흰색)
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  // 앞쪽 위 왼쪽 (회색)
		 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,	// 사각뿔 꼭짓점
		 // X축과 Y축을 위한 좌표 (추가)
		-1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, // X축
		 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, // X축
		 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, // Y축
		 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, // Y축
		 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Z축
		 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f  // Z축
	};

	// 인덱스 데이터
	GLuint indices[] = {
		// 윗면 (1번 면)
		3, 2, 6, 3, 6, 7,
		// 옆면 (2번 면)
		0, 3, 7, 0, 7, 4,
		// 옆면 (3번 면)
		1, 2, 6, 1, 6, 5,
		// 옆면 (4번 면)
		0, 1, 5, 0, 5, 4,
		// 옆면 (5번 면)
		4, 5, 6, 4, 6, 7,
		// 바닥면 (6번 면)
		0, 1, 2, 0, 2, 3,
		// 사면뿔 1
		0, 1, 8,
		// 사면뿔 2
		0, 4, 8,
		// 사면뿔 3
		4, 5, 8,
		// 사면뿔 4
		1, 5, 8,
		// X축
		9, 10,
		// Y축
		11, 12,
		// Z축
		13, 14
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

	// 위치 속성 지정 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// 색상 속성 지정 (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// VAO 언바인딩
	glBindVertexArray(0);
}
