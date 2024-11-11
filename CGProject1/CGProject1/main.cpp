#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <math.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

#define M_PI 3.141592
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
GLuint squareVao, squareVbo;
GLuint collectedVao, collectedVbo;

using namespace std;
using namespace glm;

//--- 전역 변수 선언
// 다각형 정보를 저장하는 구조체
struct ShapeInfo {
	GLuint vao, vbo, pathVbo;
	vector<GLfloat> pathVertices;
	vector<GLfloat> vertices;
	GLfloat velocityX;
	GLfloat velocityY;
	GLfloat angle;
	int numSides;
	bool onTherBar;
	int timer;
	GLfloat r, g, b;
};

vector<ShapeInfo> shapes;

bool isDrawing = false;
bool isRotate = false;
bool isAfterSpinning = false;
bool movingRight = true;
bool drawLine = false;
GLfloat mouseX = 0.0f, mouseY = 0.0f;
GLfloat ropeEndX = 0.0f, ropeEndY = 0.0f;
GLfloat plusVelocityX = 0.02f;
GLfloat minusVelocityX = -0.02f;

GLfloat barX = 0.0f;
GLfloat barY = -0.8f;
GLfloat barLength = 0.4f;
GLfloat barHeight = 0.1f;

void addShape(int sides, vector<GLfloat> vertices, GLfloat velocity) {
	ShapeInfo shape;
	shape.numSides = sides;
	shape.vertices = vertices;
	shape.velocityX = velocity;
	shape.velocityY = plusVelocityX;
	shape.angle = 0.0f;
	shape.onTherBar = false;
	shape.timer = 0;
	shape.r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	shape.g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	shape.b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	
	GLfloat gravity = -0.0005f;
	glm::vec2 center(0.0f, 0.0f);
	for (size_t i = 0; i < shape.vertices.size(); i += 2) {
		center.x += shape.vertices[i];
		center.y += shape.vertices[i + 1];
	}
	center /= (shape.numSides);
	GLfloat x = center.x;
	GLfloat y = center.y;
	GLfloat vx = shape.velocityX;
	GLfloat vy = shape.velocityY;
	for (int i = 0; i < 100; ++i) {
		// 경로의 각 점을 pathVertices에 추가
		shape.pathVertices.push_back(x);
		shape.pathVertices.push_back(y);

		// 속도와 중력에 따른 다음 위치 계산
		vy += gravity;
		x += vx;
		y += vy;

		// 화면 밖으로 나가는 경우 중단
		if (x < -1.0f || x > 1.0f || y < -1.0f) break;
	}

	// pathVBO 생성 및 초기화
	glGenBuffers(1, &shape.pathVbo);
	glBindBuffer(GL_ARRAY_BUFFER, shape.pathVbo);
	glBufferData(GL_ARRAY_BUFFER, shape.pathVertices.size() * sizeof(GLfloat), shape.pathVertices.data(), GL_STATIC_DRAW);

	// VAO, VBO 생성 및 초기화
	glGenVertexArrays(1, &shape.vao);
	glGenBuffers(1, &shape.vbo);
	glBindVertexArray(shape.vao);
	glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO 언바인딩
	shapes.push_back(shape);
}

// CCW 함수: 세 점 (A, B, C)의 방향을 체크
bool ccw(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By, GLfloat Cx, GLfloat Cy) {
	return (Cy - Ay) * (Bx - Ax) > (By - Ay) * (Cx - Ax);
}

// 두 선분이 교차하는지 확인하는 함수
bool isIntersecting(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By,
	GLfloat Cx, GLfloat Cy, GLfloat Dx, GLfloat Dy) {
	return (ccw(Ax, Ay, Cx, Cy, Dx, Dy) != ccw(Bx, By, Cx, Cy, Dx, Dy)) &&
		(ccw(Ax, Ay, Bx, By, Cx, Cy) != ccw(Ax, Ay, Bx, By, Dx, Dy));
}

// 교차점 계산 함수
bool getIntersection(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By,
	GLfloat Cx, GLfloat Cy, GLfloat Dx, GLfloat Dy,
	GLfloat& intersectionX, GLfloat& intersectionY) {
	GLfloat det = (Bx - Ax) * (Dy - Cy) - (By - Ay) * (Dx - Cx);
	if (det == 0.0f) return false; // 선분이 평행할 경우 교차하지 않음

	GLfloat t = ((Cx - Ax) * (Dy - Cy) - (Cy - Ay) * (Dx - Cx)) / det;
	intersectionX = Ax + t * (Bx - Ax);
	intersectionY = Ay + t * (By - Ay);
	return true;
}

void splitShape(const ShapeInfo& shape, GLfloat intersectionX1, GLfloat intersectionY1, GLfloat intersectionX2, GLfloat intersectionY2, vector<ShapeInfo>& newShapes) {
	vector<GLfloat> vertices1, vertices2;
	bool addToFirstPiece = true;
	bool intersectionAdded1 = false;
	bool intersectionAdded2 = false;

	for (int i = 0; i < shape.numSides; ++i) {
		int next = (i + 1) % shape.numSides;
		GLfloat Ax = shape.vertices[i * 2];
		GLfloat Ay = shape.vertices[i * 2 + 1];
		GLfloat Bx = shape.vertices[next * 2];
		GLfloat By = shape.vertices[next * 2 + 1];

		if (addToFirstPiece) {
			vertices1.push_back(Ax);
			vertices1.push_back(Ay);
		}
		else {
			vertices2.push_back(Ax);
			vertices2.push_back(Ay);
		}

		if (isIntersecting(mouseX, mouseY, ropeEndX, ropeEndY, Ax, Ay, Bx, By)) {
			GLfloat intersectionX, intersectionY;
			if (getIntersection(mouseX, mouseY, ropeEndX, ropeEndY, Ax, Ay, Bx, By, intersectionX, intersectionY)) {
				if (!intersectionAdded1) {
					vertices1.push_back(intersectionX1);
					vertices1.push_back(intersectionY1);
					vertices2.push_back(intersectionX1);
					vertices2.push_back(intersectionY1);
					intersectionAdded1 = true;
				}
				else if (!intersectionAdded2) {
					vertices1.push_back(intersectionX2);
					vertices1.push_back(intersectionY2);
					vertices2.push_back(intersectionX2);
					vertices2.push_back(intersectionY2);
					intersectionAdded2 = true;
				}
				addToFirstPiece = !addToFirstPiece;
			}
		}
	}

	ShapeInfo piece1;
	piece1.numSides = vertices1.size() / 2;
	piece1.vertices = vertices1;
	piece1.velocityX = plusVelocityX / 2;
	piece1.velocityY = 0.02f;
	piece1.onTherBar = false;
	piece1.timer = 0;
	piece1.r = shape.r;
	piece1.g = shape.g;
	piece1.b = shape.b;

	glGenVertexArrays(1, &piece1.vao);
	glGenBuffers(1, &piece1.vbo);
	glBindVertexArray(piece1.vao);
	glBindBuffer(GL_ARRAY_BUFFER, piece1.vbo);
	glBufferData(GL_ARRAY_BUFFER, piece1.vertices.size() * sizeof(GLfloat), piece1.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	ShapeInfo piece2;
	piece2.numSides = vertices2.size() / 2;
	piece2.vertices = vertices2;
	piece2.velocityX = minusVelocityX / 2;
	piece2.velocityY = 0.02f;
	piece2.onTherBar = false;
	piece2.timer = 0;
	piece2.r = shape.r;
	piece2.g = shape.g;
	piece2.b = shape.b;

	glGenVertexArrays(1, &piece2.vao);
	glGenBuffers(1, &piece2.vbo);
	glBindVertexArray(piece2.vao);
	glBindBuffer(GL_ARRAY_BUFFER, piece2.vbo);
	glBufferData(GL_ARRAY_BUFFER, piece2.vertices.size() * sizeof(GLfloat), piece2.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	newShapes.push_back(piece1);
	newShapes.push_back(piece2);
}

bool checkCollisionWithShape(ShapeInfo& shape, GLfloat ropeStartX, GLfloat ropeStartY, GLfloat ropeEndX, GLfloat ropeEndY,
	GLfloat& intersectionX1, GLfloat& intersectionY1, GLfloat& intersectionX2, GLfloat& intersectionY2) {
	bool foundFirstIntersection = false;

	for (int i = 0; i < shape.numSides; ++i) {
		int next = (i + 1) % shape.numSides;
		GLfloat Ax = shape.vertices[i * 2];
		GLfloat Ay = shape.vertices[i * 2 + 1];
		GLfloat Bx = shape.vertices[next * 2];
		GLfloat By = shape.vertices[next * 2 + 1];

		if (isIntersecting(ropeStartX, ropeStartY, ropeEndX, ropeEndY, Ax, Ay, Bx, By)) {
			GLfloat intersectionX, intersectionY;
			if (getIntersection(ropeStartX, ropeStartY, ropeEndX, ropeEndY, Ax, Ay, Bx, By, intersectionX, intersectionY)) {
				if (!foundFirstIntersection) {
					// 첫 번째 교차점 발견
					intersectionX1 = intersectionX;
					intersectionY1 = intersectionY;
					foundFirstIntersection = true;
				}
				else {
					// 두 번째 교차점 발견
					intersectionX2 = intersectionX;
					intersectionY2 = intersectionY;
					return true; // 두 개의 교차점이 모두 발견되면 true 반환
				}
			}
		}
	}
	return false; // 두 개의 교차점을 찾지 못한 경우
}

void checkCollisionAndSlice(GLfloat ropeStartX, GLfloat ropeStartY, GLfloat ropeEndX, GLfloat ropeEndY) {
	auto it = shapes.begin();
	vector<ShapeInfo> newShapes;

	while (it != shapes.end()) {
		ShapeInfo& shape = *it;
		GLfloat intersectionX1, intersectionY1, intersectionX2, intersectionY2;

		if (checkCollisionWithShape(shape, ropeStartX, ropeStartY, ropeEndX, ropeEndY, intersectionX1, intersectionY1, intersectionX2, intersectionY2)) {
			splitShape(shape, intersectionX1, intersectionY1, intersectionX2, intersectionY2, newShapes);
			it = shapes.erase(it);
		}
		else {
			++it;
		}
	}

	shapes.insert(shapes.end(), newShapes.begin(), newShapes.end());
}

bool checkCollisionWithBar(const ShapeInfo& shape, GLfloat barX, GLfloat barY, GLfloat barLength, GLfloat barHeight) {
	GLfloat barLeft = barX - barLength / 2;
	GLfloat barRight = barX + barLength / 2;
	GLfloat barBottom = barY - barHeight / 2;
	GLfloat barTop = barY + barHeight / 2;

	for (size_t i = 0; i < shape.vertices.size(); i += 2) {
		GLfloat vertexX = shape.vertices[i];
		GLfloat vertexY = shape.vertices[i + 1];

		if (vertexX >= barLeft && vertexX <= barRight && vertexY >= barBottom && vertexY <= barTop) {
			return true;
		}
	}
	return false;
}

void updateLine() {
	GLfloat vertices[] = {
		mouseX, mouseY,       // 시작점
		ropeEndX, ropeEndY    // 끝점
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateBar() {
	GLfloat barVertices[] = {
		barX - barLength / 2, barY - barHeight / 2, // 왼쪽 아래
		barX + barLength / 2, barY - barHeight / 2, // 오른쪽 아래
		barX + barLength / 2, barY + barHeight / 2, // 오른쪽 위
		barX - barLength / 2, barY + barHeight / 2  // 왼쪽 위
	};

	glBindBuffer(GL_ARRAY_BUFFER, squareVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(barVertices), barVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mouse(int button, int state, int x, int y) {
	// printf("x = %d, y = %d\n", x, y); // 디버그 메시지 출력
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseX = (2.0f * x / WINDOW_X) - 1.0f;
			mouseY = 1.0f - (2.0f * y / WINDOW_Y);
			ropeEndX = mouseX;
			ropeEndY = mouseY;
			isDrawing = true;
		}
		else if (state == GLUT_UP) {
			isDrawing = false;
			checkCollisionAndSlice(mouseX, mouseY, ropeEndX, ropeEndY); // 충돌 검사 및 절단
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	if(isDrawing) {
		ropeEndX = (2.0f * x / WINDOW_X) - 1.0f;
		ropeEndY = 1.0f - (2.0f * y / WINDOW_Y);
		glutPostRedisplay();
	}
}

void timerFunc(int value) {
	// 바 이동
	if (movingRight) {
		barX += 0.01f;
		if (barX >= 0.8f) {
			movingRight = false;
		}
	}
	else {
		barX -= 0.01f;
		if (barX <= -0.8f) {
			movingRight = true;
		}
	}

	GLfloat gravity = -0.0001f;     // 중력 가속도
	auto it = shapes.begin();
	while (it != shapes.end()) {
		ShapeInfo& shape = *it;

		bool isOutside = true;

		if (isRotate) {
			shape.angle += 0.1f; // 회전 속도
			if (shape.angle > 360.0f) { shape.angle -= 360.0f; }
			//중심 계산
			glm::vec2 center(0.0f, 0.0f);
			for (size_t i = 0; i < shape.vertices.size(); i += 2) {
				center.x += shape.vertices[i];
				center.y += shape.vertices[i + 1];
			}
			center /= (shape.numSides);

			// mat로 바꿔서 회전행렬
			float angleInRadians = glm::radians(shape.angle);
			glm::mat2 rotationMatrix = glm::mat2(
				cos(angleInRadians), -sin(angleInRadians),
				sin(angleInRadians), cos(angleInRadians)
			);
			// mat를 다시 vertice로
			for (size_t i = 0; i < shape.vertices.size(); i += 2) {
				shape.velocityY += gravity;
				if (shape.onTherBar) {
					shape.timer++;
					glm::vec2 originalPos(shape.vertices[i], shape.vertices[i + 1]);
					glm::vec2 centeredPos = originalPos - center;
					glm::vec2 rotatedPos = centeredPos + center;
					if (movingRight) {
						shape.vertices[i] = rotatedPos.x + 0.01f;
					}
					else {
						shape.vertices[i] = rotatedPos.x - 0.01f;
					}
				}
				else {
					glm::vec2 originalPos(shape.vertices[i], shape.vertices[i + 1]);
					glm::vec2 centeredPos = originalPos - center;
					glm::vec2 rotatedPos = rotationMatrix * centeredPos + center;

					shape.vertices[i] = rotatedPos.x + shape.velocityX;
					shape.vertices[i + 1] = rotatedPos.y + shape.velocityY;
				}
				if (shape.vertices[i] > -1.0f && shape.vertices[i + 1] > -1.0f) {
					isOutside = false;
				}
			}
		}
		else {
			//중심 계산
			glm::vec2 center(0.0f, 0.0f);
			for (size_t i = 0; i < shape.vertices.size(); i += 2) {
				center.x += shape.vertices[i];
				center.y += shape.vertices[i + 1];
			}
			center /= (shape.numSides);

			for (size_t i = 0; i < shape.vertices.size(); i += 2) {
				shape.velocityY += gravity;

				glm::vec2 originalPos(shape.vertices[i], shape.vertices[i + 1]);
				glm::vec2 centeredPos = originalPos - center;
				glm::vec2 rotatedPos = centeredPos + center;

				if (shape.onTherBar) {
					shape.timer++;
					if (movingRight) {
						shape.vertices[i] = rotatedPos.x + 0.01f;
					}
					else {
						shape.vertices[i] = rotatedPos.x - 0.01f;
					}
				}
				else {
					shape.vertices[i] = rotatedPos.x + shape.velocityX;
					shape.vertices[i + 1] = rotatedPos.y + shape.velocityY;
				}

				if (shape.vertices[i] > -1.0f && shape.vertices[i + 1] > -1.0f) {
					isOutside = false;
				}
			}
			
		}		

		// VBO 업데이트
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertices.size() * sizeof(GLfloat), shape.vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (isOutside || shape.timer > 1000) {
			it = shapes.erase(it);
		}
		else if (checkCollisionWithBar(shape, barX, barY, barLength, barHeight)) {
			shape.velocityX = 0;
			shape.velocityY = 0;
			shape.onTherBar = true;
			++it;
		}
		else {
			++it;
		}
	}

	// 도형 추가되는 타이머
	static int shapeSpawnTimer = 0;
	shapeSpawnTimer += 1;
	if (shapeSpawnTimer > 60) {
		shapeSpawnTimer = 0;

		GLfloat posX = (rand() % 2 == 0) ? -1.0f : 1.0f;
		GLfloat posY = (static_cast<float>(rand()) / RAND_MAX * 0.5f) - 0.25f;
		GLfloat velocity;
		if (posX == -1) {
			velocity = plusVelocityX;
		}
		else {
			velocity = minusVelocityX;
		}

		int numSides = 3 + rand() % 6;

		vector<GLfloat> vertices;
		GLfloat radius = 0.1f + static_cast<float>(rand()) / RAND_MAX * 0.05f;

		for (int i = 0; i < numSides; ++i) {
			GLfloat angle = i * 2.0f * M_PI / numSides;
			GLfloat x = posX + radius * cos(angle);
			GLfloat y = posY + radius * sin(angle);
			vertices.push_back(x);
			vertices.push_back(y);
		}

		addShape(numSides, vertices, velocity);
	}

	// 화면 업데이트 요청
	glutPostRedisplay();
	glutTimerFunc(16, timerFunc, 0);
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // 디버그 메시지 출력
	switch(key) {
	case '1':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	case '2':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	case '+':
	case '=':
		if (plusVelocityX < 0.03f) {
			plusVelocityX += 0.01f;
			minusVelocityX -= 0.01f;
		}
		break;
	case '-':
	case '_':
		if (plusVelocityX > 0.01f) {
			plusVelocityX -= 0.01f;
			minusVelocityX += 0.01f;
		}
		break;
	case 'r':
	case 'R':
		isRotate = !isRotate;
		break;
	case 'l':
	case 'L':
		drawLine = !drawLine;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("CGTermProject");

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
	glutMotionFunc(Motion);
	glutTimerFunc(1000, timerFunc, 0);
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
}

//--- 출력 콜백 함수
GLvoid drawScene() {
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// 예상선
	if (drawLine) {
		for (const auto& shape : shapes) {
			if (!shape.pathVertices.empty()) {
				glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.5f, 0.5f, 0.5f);

				glBindBuffer(GL_ARRAY_BUFFER, shape.pathVbo);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glDrawArrays(GL_LINE_STRIP, 0, shape.pathVertices.size() / 2);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
	}

	// 도형
	for (const auto& shape : shapes) {
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), shape.r, shape.g, shape.b);
		glBindVertexArray(shape.vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, shape.vertices.size() / 2);
		glBindVertexArray(0);
	}

	// 바
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);
	updateBar();
	glBindVertexArray(squareVao);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);

	// 절삭선 그리기
	if (isDrawing) {
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
		updateLine();
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	// 로프 VAO 및 VBO 생성 및 바인딩
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// 사각형(bar) VAO 및 VBO 생성 및 바인딩
	glGenVertexArrays(1, &squareVao);
	glBindVertexArray(squareVao);

	glGenBuffers(1, &squareVbo);
	glBindBuffer(GL_ARRAY_BUFFER, squareVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// collected VAO 및 VBO 생성 및 바인딩
	glGenVertexArrays(1, &collectedVao);
	glBindVertexArray(collectedVao);

	glGenBuffers(1, &collectedVbo);
	glBindBuffer(GL_ARRAY_BUFFER, collectedVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}
