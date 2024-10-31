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

//--- 전역 변수 선언
struct ShapeInfo {
	int shapeType;
	GLfloat x, y;
};

int shapeType;
const int MAX_SHAPES = 10;
ShapeInfo shapes[MAX_SHAPES];
int numShapes = 0;
int currentShapeIndex = 0;

GLint mousePosLocation;
GLfloat mouseX = 0.0f, mouseY = 0.0f;
GLuint vao, vbo;

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch(key) {
        case 'p':
			shapeType = 1;
			break;
		case 'l':
			shapeType = 2;
			break;
		case 't':
			shapeType = 3;
			break;
		case 'r':
			shapeType = 4;
			break;
		case 'w':
			if (numShapes > 0) {
				int randomIndex = rand() % numShapes;
				shapes[randomIndex].y += 0.2f;
				glutPostRedisplay();
			}
			break;
		case 'a':
			if (numShapes > 0) {
				int randomIndex = rand() % numShapes;
				shapes[randomIndex].x -= 0.2f;
				glutPostRedisplay();
			}
			break;
		case 's':
			if (numShapes > 0) {
				int randomIndex = rand() % numShapes;
				shapes[randomIndex].y -= 0.2f;
				glutPostRedisplay();
			}
			break;
		case 'd':
			if (numShapes > 0) {
				int randomIndex = rand() % numShapes;
				shapes[randomIndex].x += 0.2f;
				glutPostRedisplay();
			}
			break;
		case 'c':
			numShapes = 0;
			currentShapeIndex = 0;
			glutPostRedisplay();
			break;
		case 'q':
			exit(0);
			break;
	}
}

void Mouse(int button, int state, int x, int y) {
	printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
	GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
	GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			shapes[currentShapeIndex].shapeType = shapeType;
			shapes[currentShapeIndex].x = normalizedX;
			shapes[currentShapeIndex].y = normalizedY;
			currentShapeIndex = (currentShapeIndex + 1) % MAX_SHAPES;
			if (numShapes < MAX_SHAPES) {
				numShapes++;
			}
		}
	}
	glutPostRedisplay();
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
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
	glutMainLoop();
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	vertexSource = filetobuf("D:/GitHub/ComputerGrapics/Shader/Project7/vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader 파일을 불러오지 못했습니다." << std::endl;
	}
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);

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
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// VAO 바인딩
	glBindVertexArray(vao);

	// 도형 그리기
	for (int i = 0; i < numShapes; i++) {
		// 색상 설정
		if (shapes[i].shapeType == 1) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 1.0f); // 파랑
		}
		else if (shapes[i].shapeType == 2) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f); // 검정
		}
		else if (shapes[i].shapeType == 3) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f); // 빨강
		}
		else if (shapes[i].shapeType == 4) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 1.0f, 0.0f); // 노랑
		}

		// 마우스 위치에 따른 변환 적용
		if (shapes[i].shapeType == 1 ) {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x + 0.1f, shapes[i].y + 0.1f);
		}
		else if (shapes[i].shapeType == 2) {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x, shapes[i].y + 0.1f);
		}
		// 삼각형과 사각형은 마우스 위치에 정점 위치를 더해서 그리기
		else {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x, shapes[i].y);
		}

		// 도형 그리기
		switch (shapes[i].shapeType) {
		case 1: // 점
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 2: // 선
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 3: // 삼각형
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 4: // 사각형
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 사각형 그리기
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
		-0.1f,  0.1f  // 왼쪽 위
	};

	// 인덱스 데이터
	GLuint indices[] = {
		0, 1, 2,  // 첫 번째 삼각형
		0, 2, 3   // 두 번째 삼각형
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


