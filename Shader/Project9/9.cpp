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

//--- ���� ���� ����
struct Colors {
	GLfloat r, g, b;
};

Colors colors[4] = {
	{0.0f, 0.0f, 1.0f},  // �Ķ�
	{0.0f, 0.0f, 0.0f},  // ����
	{1.0f, 0.0f, 0.0f},  // ����
	{1.0f, 1.0f, 0.0f}   // ���
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
GLfloat spiralStep = 10.0f; // ���� ���� �̵� �Ÿ�
int spiralDirection = 0;   // ���� �̵� ���� (0: ������, 1: �Ʒ���, 2: ����, 3: ����)
GLfloat spiralShrink = 2.0f; // �����̷� ��� ����

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
			// �ﰢ�� ��ġ ������Ʈ
			matrix[i][0].x += directionX[i] * speed;
			matrix[i][0].y += directionY[i] * speed;
			
			// �� �浹 ó�� - X��
			if (matrix[i][0].x >= 1.0f) {
				directionX[i] = -1; // X ���� ����
				matrix[i][0].x = 1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}
			else if (matrix[i][0].x <= -1.0f) {
				directionX[i] = 1; // X ���� ����
				matrix[i][0].x = -1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}

			// �� �浹 ó�� - Y��
			if (matrix[i][0].y >= 1.0f) {
				directionY[i] = -1; // Y ���� ����
				matrix[i][0].y = 1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}
			else if (matrix[i][0].y <= -1.0f) {
				directionY[i] = 1; // Y ���� ����
				matrix[i][0].y = -1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}

			// ���� ����: �̵� ���⿡ ���� ����
			if (directionX[i] == 1 && directionY[i] == 1) {
				angle[i] = 45.0f;  // ������ �� ����
			}
			else if (directionX[i] == -1 && directionY[i] == 1) {
				angle[i] = 135.0f; // ���� �� ����
			}
			else if (directionX[i] == -1 && directionY[i] == -1) {
				angle[i] = 225.0f; // ���� �Ʒ� ����
			}
			else if (directionX[i] == 1 && directionY[i] == -1) {
				angle[i] = 315.0f; // ������ �Ʒ� ����
			}
		}
	}
	break;

	case 2:
	{
		for (int i = 0; i < 4; i++) {
			// �ﰢ�� ��ġ ������Ʈ
			matrix[i][0].x += directionX[i] * speed;

			// �� �浹 ó��
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

			// ȭ�� �Ʒ��ʿ� �����ϸ� ������ ���� ��ȯ
			if (matrix[i][0].y <= -1.0f) {
				directionY[i] = 1; // �������� �̵�
			}
			// ȭ�� ���ʿ� �����ϸ� ������ �Ʒ��� ��ȯ
			else if (matrix[i][0].y >= 1.0f) {
				directionY[i] = -1; // �Ʒ������� �̵�
			}
		}
	}
	break;
	case 3: 
	{
		if (spiralStep <= -0.2f) break;
		for (int i = 0; i < 4; i++) {
			switch (spiralDirection) {
			case 0: // ������
				matrix[i][0].x += speed;
				angle[i] = 90.0f;
				break;
			case 1: // �Ʒ���
				matrix[i][0].y -= speed;
				angle[i] = 180.0f;
				break;
			case 2: // ����
				matrix[i][0].x -= speed;
				angle[i] = 270.0f;
				break;
			case 3: // ����
				matrix[i][0].y += speed;
				angle[i] = 0.0f;
				break;
			}
		}
		spiralStep -= 0.1f;
		if (spiralStep <= 0.0f) {
			spiralDirection = (spiralDirection + 1) % 4; // ���� ��ȯ
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
		static GLfloat radius = 0.1f; // �ʱ� ������ ����
		static GLfloat theta = 0.0f; // ���� (����)

		for (int i = 0; i < 4; i++) {
			theta += speed;

			matrix[i][0].x = radius * cos(theta);
			matrix[i][0].y = radius * sin(theta);
			angle[i] = theta * 180.0f / M_PI; // ������ ����

			radius += 0.0005f;

			// ȭ�� ������ ����� ������ �ʱ�ȭ
			if (radius > 1.5f) {
				radius = 0.1f;
				theta = 0.0f;
			}
		}
	}
	break;

	}

	// ȭ�� ������Ʈ
	glutPostRedisplay();
	// Ÿ�̸� ���� (�ٽ� ȣ��)
	glutTimerFunc(16, Timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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
		matrix[0][0].x = 0.0f; matrix[0][0].y = 1.0f;  // 1��и�
		matrix[1][0].x = -1.0f;  matrix[1][0].y = 1.0f;  // 2��и�
		matrix[2][0].x = -1.0f; matrix[2][0].y = 0.0f; // 3��и�
		matrix[3][0].x = 0.0f;  matrix[3][0].y = 0.0f; // 4��и�
		maxStep = 10.0f;
		spiralStep = 10.0f;
		spiralDirection = 0;
		spiralShrink = 2.0f;
		moveMode = 3;
		break;
	case '4':
		matrix[0][0].x = 0.5f; matrix[0][0].y = 0.5f;  // 1��и�
		matrix[1][0].x = -0.5f;  matrix[1][0].y = 0.5f;  // 2��и�
		matrix[2][0].x = -0.5f; matrix[2][0].y = -0.5f; // 3��и�
		matrix[3][0].x = 0.5f;  matrix[3][0].y = -0.5f; // 4��и�
		moveMode = 4;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}

void Mouse(int button, int state, int x, int y) {
	//printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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
	matrix[0][0].x = 0.5f; matrix[0][0].y = 0.5f;  // 1��и�
	matrix[1][0].x = -0.5f;  matrix[1][0].y = 0.5f;  // 2��и�
	matrix[2][0].x = -0.5f; matrix[2][0].y = -0.5f; // 3��и�
	matrix[3][0].x = 0.5f;  matrix[3][0].y = -0.5f; // 4��и�
}

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");
	initializeTrianglePositions();

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

//--- ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader ������ �ҷ����� ���߽��ϴ�." << std::endl;
	}
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//checkCompileErrors(vertexShader, "VERTEX");
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

	// ���콺 ��ġ uniform ���� ��ġ ���
	mousePosLocation = glGetUniformLocation(shaderProgramID, "mousePos");
}

//--- ��� �ݹ� �Լ�
//--- ��� �ݹ� �Լ�
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);  // �ݵ�� ���̴� ���α׷� Ȱ��ȭ

	// VAO ���ε�
	glBindVertexArray(vao);
	/*
	// ���ڼ� �׸���
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
	glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), 0.0f, 0.0f);  // ���ڼ��� �߾ӿ� �׸���
	glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), 0.0f);
	glDrawArrays(GL_LINES, 3, 2);  // ���� �� �׸���
	glDrawArrays(GL_LINES, 5, 2);  // ���� �� �׸���
	*/

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < numShape[i]; j++) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), colors[i].r, colors[i].g, colors[i].b);
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i][j].x, matrix[i][j].y);
			glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), angle[i] * M_PI / 180.0f); // ������ �������� ��ȯ

			// �ﰢ�� �׸���
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}

	// VAO ����ε�
	glBindVertexArray(0);

	// ȭ�鿡 ���
	glutSwapBuffers();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	// ���� ������: �ִ� 4���� ���� ����
	GLfloat vertices[] = {
		// x, y ��ǥ
		0.0f, 0.1f,   // ��� ��
		-0.1f, -0.1f, // ���� �Ʒ�
		0.1f, -0.1f,  // ������ �Ʒ�
		-1.0f,  0.0f, // ���� �� ���� ��
		1.0f,  0.0f,  // ���� �� ������ ��
		0.0f,  1.0f,  // ���� �� ��� ��
		0.0f, -1.0f   // ���� �� �ϴ� ��

	};

	// �ε��� ������
	GLuint indices[] = {
		0, 1, 2,	// �ﰢ�� �׸���
		5, 6,     // ���� ��
		7, 8      // ���� ��
	};

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) ���� �� ������ ����
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO ����ε�
	glBindVertexArray(0);
}


