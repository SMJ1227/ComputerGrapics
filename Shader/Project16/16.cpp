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
GLuint spiralVao, spiralVbo;
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

int isSpiral = 0;
int spiralIndex = 0;
GLfloat spiralPoints[300];

int isMoving = 0;
GLfloat movingX = 0.0f;
GLfloat movingY = 0.0f;
GLfloat movingZ = 0.0f;
GLfloat KeymovingX = 0.0f;
GLfloat KeymovingY = 0.0f;
GLfloat KeymovingZ = 0.0f;

GLfloat rotationAngle = 0.0f;
int timerY = 0;
int jOrg = 0;	// 자전 or 공전
int changeRotate = 0;
int gxyz = 0;
int isScaling = 0;
GLfloat scaling = 0.0f;
int initialScaling = 0;
GLfloat initialX = 1.0f;
GLfloat initialY = 0.0f;
GLfloat initialZ = 0.0f;

void setupCamera() {
	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
}

// 타이머 함수
void timerFunc(int value) {
	if (isScaling == 1) {
		scaling += 0.01f;
		if (scaling > 0.5f) {
			isScaling = 2;
		}
	}
	else if (isScaling == 2) {
		scaling -= 0.01f;
		if (scaling < 0.0f) {
			isScaling = 1;
		}
	}

	if (initialScaling == 1) {
		scaling += 0.01f;
		if (scaling > 0.5f) {
			initialScaling = 2;
		}
	}
	else if (isScaling == 2 || initialScaling == 2) {
		scaling -= 0.01f;
		if (scaling < 0.0f) {
			initialScaling = 1;
		}
	}

	if (isSpiral) {
		// 스파이럴 포인트 배열에서 다음 좌표 가져오기
		movingX = spiralPoints[spiralIndex * 3];
		movingY = spiralPoints[spiralIndex * 3 + 1];
		movingZ = spiralPoints[spiralIndex * 3 + 2];

		spiralIndex++; // 다음 포인트로 이동
		if (spiralIndex >= 100) spiralIndex = 0; // 스파이럴 배열 순환
	}
	else {
		movingX += 0.05f;
		if (movingX > 2.0f) {
			movingX = 0;
			isMoving = 0;
		}
	}

	rotationAngle += radians(3.0f);
	if (changeRotate) {
		changeRotate++;
		if (changeRotate >= 60) {
			changeRotate = 0;
			timerY = 0;
			jOrg = 0;
		}
	}
	glutPostRedisplay();  // 화면 업데이트 요청
	glutTimerFunc(100, timerFunc, 0);  // 0.1초마다 타이머 재호출
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case 'a':
		isScaling = 1;
		break;
	case 'd':
		initialScaling = 1;
		break;
	case '1': // 키보드 1 : xz 평면에 스파이럴을 그리고, 그 스파이럴 위치에 따라 객체 이동 애니메이션
		isSpiral = 1;
		break;
	case '2': //키보드 2 : 두 도형이 원점을 통과하며 상대방의 자리로 이동하는 애니메이션
		isMoving = 1;
		movingX = 0;
		break;
	case '3': //키보드 3 : 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션
		isMoving = 0;
		rotationAngle = 0;
		timerY = 0;
		jOrg = 1;
		changeRotate = 1;
		gxyz = 0;
		break;
	case '4': // 키보드 4 : 두 도형이 한 개는 위로, 다른 도형은 아래로 이동하면서 상대방의 자리로 이동하는 애니메이션
		isMoving = 0;
		rotationAngle = 0;
		timerY = 0;
		jOrg = 1;
		changeRotate = 1;
		gxyz = 1;
		break;
	case '5': // 키보드 5 : 두 도형이 한 개는 확대, 다른 한 개는 축소되며 자전과 공전하기
		isMoving = 0; 
		timerY = 1;
		jOrg = 1;
		gxyz = 0;
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
	case 'f':
		KeymovingX -= 0.1f;
		break;
	case 'g':
		KeymovingZ -= 0.1f;
		break;
	case 'h':
		KeymovingX += 0.1f;
		break;
	case 't':
		KeymovingZ += 0.1f;
		break;
	case 'r':
		KeymovingY += 0.1f;
		break;
	case 'y':
		KeymovingY -= 0.1f;
		break;
	case 's':
		initialScaling = 0;
		isScaling = 0;
		isMoving = 0;
		movingX = 0;
		rotationAngle = 0.0f;
		timerY = 0;
		jOrg = 0;
		gxyz = 0;
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

void initSpiralData() {
	const int numPoints = 100;  // 스파이럴을 형성할 점의 수를 증가
	double deltaAngle = 0.2f;  // 각도 증가량을 조정하여 더 많은 점으로 스파이럴을 그림
	double deltaRadius = 0.05f;  // 반지름 증가량 감소
	double angle = 0.0f, radius = 0.0f;

	for (int i = 0; i < numPoints; ++i) {
		spiralPoints[i * 3] = radius * cos(angle);   // X
		spiralPoints[i * 3 + 1] = 0;                 // Y
		spiralPoints[i * 3 + 2] = radius * sin(angle); // Z
		angle += deltaAngle;
		radius += deltaRadius;
	}

	glGenVertexArrays(1, &spiralVao);
	glBindVertexArray(spiralVao);

	glGenBuffers(1, &spiralVbo);
	glBindBuffer(GL_ARRAY_BUFFER, spiralVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spiralPoints), spiralPoints, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO 언바인딩
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
	initSpiralData();

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
	// 그리기
	if (isSpiral) {
		glBindVertexArray(spiralVao);
		glDrawArrays(GL_LINE_STRIP, 0, 100);
		glBindVertexArray(0); // VAO 언바인딩
	}
	// VAO 바인딩
	glBindVertexArray(vao);

	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 1.0f, 0.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	mat4 model = mat4(1.0f);
	if (jOrg == 0) {
		if (isMoving) {
			model = translate(model, vec3(1.0f - movingX, 0.0f, 0.0f));
		}
		else if (isSpiral) {
			model = translate(model, vec3(movingX, movingY, movingZ));
		}
		else {
			model = translate(model, vec3(1.0f, 0.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (timerY == 1) {
			model = rotate(model, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
			model = translate(model, vec3(1.0f, 0.0f, 0.0f));		
			model = scale(model, vec3(0.5, 0.5, 0.5));
		}
		if (timerY == 0) {
			if (!gxyz) {
				float r = 1.0f; // 공전 반지름
				float x = r * cos(rotationAngle);
				float z = -r * sin(rotationAngle);
				model = translate(model, vec3(x, 0.0f, z));
			}
			if (gxyz) {
				float r = 1.0f; // 공전 반지름
				float x = r * cos(rotationAngle);
				float y = -r * sin(rotationAngle);
				model = translate(model, vec3(x, y, 0.0f));
			}
		}
	}
	if (isScaling) {
		model = scale(model, vec3(scaling, scaling, scaling));
	}
	if (initialScaling) {
		model = translate(model, vec3(-initialX, -initialY, -initialZ));
		model = scale(model, vec3(scaling, scaling, scaling));
		model = translate(model, vec3(initialX, initialY, initialZ));
	}
	model = translate(model, vec3(KeymovingX, KeymovingY, KeymovingZ));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	mat4 model2 = mat4(1.0f);
	if (jOrg == 0) {
		if (isMoving) {
			model2 = translate(model2, vec3(-1.0f + movingX, 0.0f, 0.0f));
		}
		else if (isSpiral) {
			model2 = translate(model2, vec3(movingX, movingY, movingZ));
		}
		else {
			model2 = translate(model2, vec3(-1.0f, 0.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (timerY == 1) {
			model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
			model2 = translate(model2, vec3(-1.0f, 0.0f, 0.0f));
			model2 = scale(model2, vec3(2.0, 2.0, 2.0));
		}
		if (timerY == 0) {
			if (!gxyz) {
				float r = 1.0f; // 공전 반지름
				float initialAngle = 3.14159265;
				float x = r * cos(rotationAngle + initialAngle);
				float z = -r * sin(rotationAngle + initialAngle);
				model2 = translate(model2, vec3(x, 0.0f, z));
			}
			if (gxyz) {
				float r = 1.0f; // 공전 반지름
				float initialAngle = 3.14159265;
				float x = r * cos(rotationAngle + initialAngle);
				float y = -r * sin(rotationAngle + initialAngle);
				model2 = translate(model2, vec3(x, y, 0.0f));
			}
		}
	}
	if (isScaling) {
		model2 = scale(model2, vec3(scaling, scaling, scaling));
	}
	model2 = translate(model2, vec3(KeymovingX, KeymovingY, KeymovingZ));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
	qobj = gluNewQuadric(); // 객체 생성하기
	gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
	gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
	gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
	gluSphere(qobj, 0.5, 50, 50); // 객체 만들기

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
		-1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // X축
		 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // X축
		 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Y축
		 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Y축
		 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Z축
		 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Z축
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
