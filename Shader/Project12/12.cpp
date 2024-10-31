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
struct Matrix {
	GLfloat x, y;
	int shapeType;
	GLfloat r, g, b;
	int directionX, directionY;
	int moveMode;
};

int matrixs = 15;
Matrix matrix[15];

void initializeTrianglePositions() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++) {
			matrix[i * 3 + j].x = ((GLfloat)rand() / RAND_MAX) * 2.0f - 1.0f;
			matrix[i * 3 + j].y = ((GLfloat)rand() / RAND_MAX) * 2.0f - 1.0f;
			matrix[i * 3 + j].shapeType = i + 1;
			matrix[i * 3 + j].r = (GLfloat)rand() / RAND_MAX;
			matrix[i * 3 + j].g = (GLfloat)rand() / RAND_MAX;
			matrix[i * 3 + j].b = (GLfloat)rand() / RAND_MAX;
			matrix[i * 3 + j].directionX = 0;
			matrix[i * 3 + j].directionY = 0;
		}
	}
}

int draggingRect = -1;
GLfloat offsetX = 0.0f, offsetY = 0.0f;
GLfloat mouseX = 0.0f, mouseY = 0.0f;

GLfloat speed = 0.05f;

GLfloat angle[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

// 타이머 함수
void timerFunc(int value) {
	for (int i = 0; i < matrixs; i++) {
		switch (matrix[i].moveMode)
		{
		case 0:
			// 삼각형 위치 업데이트
			matrix[i].x += matrix[i].directionX * speed;
			matrix[i].y += matrix[i].directionY * speed;

			// 벽 충돌 처리 - X축
			if (matrix[i].x >= 1.0f) {
				matrix[i].directionX = -1; // X 방향 반전
				matrix[i].x = 1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}
			else if (matrix[i].x <= -1.0f) {
				matrix[i].directionX = 1; // X 방향 반전
				matrix[i].x = -1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}

			// 벽 충돌 처리 - Y축
			if (matrix[i].y >= 1.0f) {
				matrix[i].directionY = -1; // Y 방향 반전
				matrix[i].y = 1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}
			else if (matrix[i].y <= -1.0f) {
				matrix[i].directionY = 1; // Y 방향 반전
				matrix[i].y = -1.0f; // 벽 밖으로 나가지 않도록 위치 조정
			}

			// 각도 설정: 이동 방향에 따라 설정
			if (matrix[i].directionX == 1 && matrix[i].directionY == 1) {
				angle[i] = 45.0f;  // 오른쪽 위 방향
			}
			else if (matrix[i].directionX == -1 && matrix[i].directionY == 1) {
				angle[i] = 135.0f; // 왼쪽 위 방향
			}
			else if (matrix[i].directionX == -1 && matrix[i].directionY == -1) {
				angle[i] = 225.0f; // 왼쪽 아래 방향
			}
			else if (matrix[i].directionX == 1 && matrix[i].directionY == -1) {
				angle[i] = 315.0f; // 오른쪽 아래 방향
			}

			break;
		case 1:
			// 위치 업데이트
			matrix[i].x += matrix[i].directionX * speed;

			// 벽 충돌 처리
			if (matrix[i].x >= 1.0f) {
				matrix[i].directionX = -1;
				matrix[i].x = 1.0f;
				if (matrix[i].directionY == 1) matrix[i].y += 0.2f;
				else matrix[i].y -= 0.2f;
			}
			else if (matrix[i].x <= -1.0f) {
				matrix[i].directionX = 1;
				matrix[i].x = -1.0f;
				if (matrix[i].directionY == 1) matrix[i].y += 0.2f;
				else matrix[i].y -= 0.2f;
			}

			// 화면 아래쪽에 도달하면 방향을 위로 전환
			if (matrix[i].y <= -1.0f) {
				matrix[i].directionY = 1; // 위쪽으로 이동
			}
			// 화면 위쪽에 도달하면 방향을 아래로 전환
			else if (matrix[i].y >= 1.0f) {
				matrix[i].directionY = -1; // 아래쪽으로 이동
			}
			break;
		default:
			break;
		}
	}

	glutPostRedisplay();  // 화면 업데이트 요청
	glutTimerFunc(100, timerFunc, 0);  // 0.1초마다 타이머 재호출
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch (key) {
	case 'r':
	case 'R':
		initializeTrianglePositions();
		matrixs = 15;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

int isInsideShape(Matrix *matrix) {
	return (mouseX >= (matrix->x - 0.1f) && mouseX <= (matrix->x + 0.1f) &&
		mouseY >= (matrix->y - 0.1f) && mouseY <= (matrix->y + 0.1f));
}

int isOverlap(Matrix* r1, Matrix* r2) {
	return !((r1->x + 0.1f) < (r2->x - 0.1f) ||	(r1->x - 0.1f) > (r2->x + 0.1f) ||
		(r1->y + 0.1f) < (r2->y - 0.1f) || (r1->y - 0.1f) > (r2->y + 0.1f));
}

void mergeShapes(int index1, int index2) {
	if (index1 > index2) {
		// 항상 index1이 더 작은 인덱스를 가리키도록 함
		int temp = index1;
		index1 = index2;
		index2 = temp;
	}

	matrix[index1].x = matrix[index2].x;
	matrix[index1].y = matrix[index2].y;
	matrix[index1].r = (GLfloat)rand() / RAND_MAX;
	matrix[index1].g = (GLfloat)rand() / RAND_MAX;
	matrix[index1].b = (GLfloat)rand() / RAND_MAX;

	if ((matrix[index1].shapeType + matrix[index2].shapeType) <= 5) {
		matrix[index1].shapeType = (matrix[index1].shapeType + matrix[index2].shapeType);
	}
	else if ((matrix[index1].shapeType + matrix[index2].shapeType) % 5 == 0) {
		matrix[index1].shapeType = 1;
	}
	else {
		matrix[index1].shapeType = (matrix[index1].shapeType + matrix[index2].shapeType) % 5;
	}

	matrix[index1].directionX = 1;
	matrix[index1].directionY = 1;
	matrix[index1].moveMode = rand() % 2;

	for (int j = index2; j < matrixs - 1; j++) {
		matrix[j] = matrix[j + 1];
	}
	matrixs--;
}

void Mouse(int button, int state, int x, int y) {
	//printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseX = (2.0f * x / WINDOW_X) - 1.0f;
			mouseY = 1.0f - (2.0f * y / WINDOW_Y);
			for (int i = 0; i < matrixs; i++) {
				if (isInsideShape(&matrix[i])) {
					draggingRect = i;
					offsetX = mouseX - matrix[i].x;
					offsetY = mouseY - matrix[i].y;
					break;
				}
			}
		}
		else if (state == GLUT_UP) {
			if (draggingRect != -1) {
				for (int i = 0; i < matrixs; i++) {
					if (i != draggingRect && isOverlap(&matrix[draggingRect], &matrix[i])) {
						//printf("overlaped %d, %d", draggingRect, i);
						mergeShapes(draggingRect, i);
						break;
					}
				}
				// 드래그 종료
				draggingRect = -1;
			}
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	if (draggingRect != -1) {
		GLfloat normalizedX = (2.0f * x / WINDOW_X) - 1.0f;
		GLfloat normalizedY = 1.0f - (2.0f * y / WINDOW_Y);

		matrix[draggingRect].x = normalizedX - offsetX;
		matrix[draggingRect].y = normalizedY - offsetY;

		glutPostRedisplay(); // 화면 갱신 요청
	}
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(NULL);
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
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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

	for (int i = 0; i < matrixs; i++) {
		// 각 사분면에 맞는 색 설정
		int color = rand() % 4;
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), matrix[i].r, matrix[i].g, matrix[i].b);

		switch (matrix[i].shapeType) {
		case 1:
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x + 0.1f, matrix[i].y + 0.1f);
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 2: // 선
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y + 0.1f);
			glDrawArrays(GL_LINES, 0, 2);

			break;
		case 3: // 삼각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 4: // 사각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			break;
		case 5: // 오각형
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 5);
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
