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

//--- �Ʒ� 5�� �Լ��� ����� ���� �Լ�
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
		perror("ERROR: ���̴� ������ �� �� �����ϴ�.");
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

//--- ���̴� ���� ����
GLint width, height;
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; //--- ���̴� ���α׷�
GLuint vao, vbo;
GLuint squareVao, squareVbo;
GLuint collectedVao, collectedVbo;

using namespace std;
using namespace glm;

//--- ���� ���� ����
// �ٰ��� ������ �����ϴ� ����ü
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
		// ����� �� ���� pathVertices�� �߰�
		shape.pathVertices.push_back(x);
		shape.pathVertices.push_back(y);

		// �ӵ��� �߷¿� ���� ���� ��ġ ���
		vy += gravity;
		x += vx;
		y += vy;

		// ȭ�� ������ ������ ��� �ߴ�
		if (x < -1.0f || x > 1.0f || y < -1.0f) break;
	}

	// pathVBO ���� �� �ʱ�ȭ
	glGenBuffers(1, &shape.pathVbo);
	glBindBuffer(GL_ARRAY_BUFFER, shape.pathVbo);
	glBufferData(GL_ARRAY_BUFFER, shape.pathVertices.size() * sizeof(GLfloat), shape.pathVertices.data(), GL_STATIC_DRAW);

	// VAO, VBO ���� �� �ʱ�ȭ
	glGenVertexArrays(1, &shape.vao);
	glGenBuffers(1, &shape.vbo);
	glBindVertexArray(shape.vao);
	glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO ����ε�
	shapes.push_back(shape);
}

// CCW �Լ�: �� �� (A, B, C)�� ������ üũ
bool ccw(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By, GLfloat Cx, GLfloat Cy) {
	return (Cy - Ay) * (Bx - Ax) > (By - Ay) * (Cx - Ax);
}

// �� ������ �����ϴ��� Ȯ���ϴ� �Լ�
bool isIntersecting(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By,
	GLfloat Cx, GLfloat Cy, GLfloat Dx, GLfloat Dy) {
	return (ccw(Ax, Ay, Cx, Cy, Dx, Dy) != ccw(Bx, By, Cx, Cy, Dx, Dy)) &&
		(ccw(Ax, Ay, Bx, By, Cx, Cy) != ccw(Ax, Ay, Bx, By, Dx, Dy));
}

// ������ ��� �Լ�
bool getIntersection(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By,
	GLfloat Cx, GLfloat Cy, GLfloat Dx, GLfloat Dy,
	GLfloat& intersectionX, GLfloat& intersectionY) {
	GLfloat det = (Bx - Ax) * (Dy - Cy) - (By - Ay) * (Dx - Cx);
	if (det == 0.0f) return false; // ������ ������ ��� �������� ����

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
					// ù ��° ������ �߰�
					intersectionX1 = intersectionX;
					intersectionY1 = intersectionY;
					foundFirstIntersection = true;
				}
				else {
					// �� ��° ������ �߰�
					intersectionX2 = intersectionX;
					intersectionY2 = intersectionY;
					return true; // �� ���� �������� ��� �߰ߵǸ� true ��ȯ
				}
			}
		}
	}
	return false; // �� ���� �������� ã�� ���� ���
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
		mouseX, mouseY,       // ������
		ropeEndX, ropeEndY    // ����
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateBar() {
	GLfloat barVertices[] = {
		barX - barLength / 2, barY - barHeight / 2, // ���� �Ʒ�
		barX + barLength / 2, barY - barHeight / 2, // ������ �Ʒ�
		barX + barLength / 2, barY + barHeight / 2, // ������ ��
		barX - barLength / 2, barY + barHeight / 2  // ���� ��
	};

	glBindBuffer(GL_ARRAY_BUFFER, squareVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(barVertices), barVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mouse(int button, int state, int x, int y) {
	// printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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
			checkCollisionAndSlice(mouseX, mouseY, ropeEndX, ropeEndY); // �浹 �˻� �� ����
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
	// �� �̵�
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

	GLfloat gravity = -0.0001f;     // �߷� ���ӵ�
	auto it = shapes.begin();
	while (it != shapes.end()) {
		ShapeInfo& shape = *it;

		bool isOutside = true;

		if (isRotate) {
			shape.angle += 0.1f; // ȸ�� �ӵ�
			if (shape.angle > 360.0f) { shape.angle -= 360.0f; }
			//�߽� ���
			glm::vec2 center(0.0f, 0.0f);
			for (size_t i = 0; i < shape.vertices.size(); i += 2) {
				center.x += shape.vertices[i];
				center.y += shape.vertices[i + 1];
			}
			center /= (shape.numSides);

			// mat�� �ٲ㼭 ȸ�����
			float angleInRadians = glm::radians(shape.angle);
			glm::mat2 rotationMatrix = glm::mat2(
				cos(angleInRadians), -sin(angleInRadians),
				sin(angleInRadians), cos(angleInRadians)
			);
			// mat�� �ٽ� vertice��
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
			//�߽� ���
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

		// VBO ������Ʈ
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

	// ���� �߰��Ǵ� Ÿ�̸�
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

	// ȭ�� ������Ʈ ��û
	glutPostRedisplay();
	glutTimerFunc(16, timerFunc, 0);
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(time(NULL));
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("CGTermProject");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram();
	Initbuffer();

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard); // Ű �Է� �ݹ�
	glutMouseFunc(Mouse); // ���콺 ��ġ �ݹ�
	glutMotionFunc(Motion);
	glutTimerFunc(1000, timerFunc, 0);
	glutMainLoop();
}

//--- ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	vertexSource = filetobuf("D:/GitHub/ComputerGrapics/Shader/Project7/vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader ������ �ҷ����� ���߽��ϴ�." << std::endl;
	}
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);

	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

//--- �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	if (!fragmentSource) {
		std::cerr << "ERROR: fragment shader ������ �ҷ����� ���߽��ϴ�." << std::endl;
	}
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

//--- ���̴� ���α׷� ����� ���̴� ��ü ��ũ�ϱ�
void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����

	//-- shader Program
	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);


	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}

//--- ��� �ݹ� �Լ�
GLvoid drawScene() {
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// ����
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

	// ����
	for (const auto& shape : shapes) {
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), shape.r, shape.g, shape.b);
		glBindVertexArray(shape.vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, shape.vertices.size() / 2);
		glBindVertexArray(0);
	}

	// ��
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);
	updateBar();
	glBindVertexArray(squareVao);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);

	// ���輱 �׸���
	if (isDrawing) {
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
		updateLine();
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	glutSwapBuffers();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	// ���� VAO �� VBO ���� �� ���ε�
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// �簢��(bar) VAO �� VBO ���� �� ���ε�
	glGenVertexArrays(1, &squareVao);
	glBindVertexArray(squareVao);

	glGenBuffers(1, &squareVbo);
	glBindBuffer(GL_ARRAY_BUFFER, squareVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// collected VAO �� VBO ���� �� ���ε�
	glGenVertexArrays(1, &collectedVao);
	glBindVertexArray(collectedVao);

	glGenBuffers(1, &collectedVbo);
	glBindBuffer(GL_ARRAY_BUFFER, collectedVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}
