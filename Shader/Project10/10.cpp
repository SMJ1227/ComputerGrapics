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

GLfloat vertices[200 * 2]; // x, y ��ǥ�� ������ ���� �迭

void Timer(int value) {
	if (currentPointIndex < numPoints) {
		currentPointIndex++;
	}

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 0);
}
void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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
	printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Ŭ���� ��ġ�� ������ �׸���
		mouseX = (2.0f * x / WINDOW_X) - 1.0f;
		mouseY = 1.0f - (2.0f * y / WINDOW_Y);
		points[0].x = mouseX;
		points[0].y = mouseY;
		currentPointIndex = 0;
		randomizeColor();
		if (drawSpiral < 1) {
			drawSpiral = 1;
		}
		glutPostRedisplay(); // ȭ���� �ٽ� �׸��� ���� ȣ��
	}
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

GLvoid drawScene() {
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);  // ���̴� ���α׷� Ȱ��ȭ

	for (int i = 0; i < drawSpiral; i++) {
		if (drawMode == 0) {
			// ù ��° ���� (Ŀ���� ����)
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), points[i].x, points[i].y);
			glUniform1f(glGetUniformLocation(shaderProgramID, "rotation"), rotationAngleX);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 1.0f, 1.0f);
			glBindVertexArray(vao);
			glDrawArrays(GL_POINTS, 0, currentPointIndex);
			
			// �� ��° ���� (�۾����� ����)
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
				glDrawArrays(GL_LINE_STRIP, 0, currentPointIndex); // ���� ������ �����Ͽ� �׸���
			}
		}
		
	}

	glBindVertexArray(0);
	glutSwapBuffers();  // ȭ�� ���
}


//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Initbuffer() {
	const int numPoints = 200;
	GLfloat angleIncrement = 0.2f;
	GLfloat radius = 0.02f;

	// ù ��° ���� (ù 95���� ��)
	for (int i = 0; i < 95; i++) {
		GLfloat angle = i * angleIncrement;
		vertices[i * 2] = radius * cos(angle);   // x ��ǥ
		vertices[i * 2 + 1] = radius * sin(angle); // y ��ǥ
		radius += 0.003f; // ������ ������ ����
	}

	// ù ��° ������ 95��° �� ��ǥ
	GLfloat lastX = vertices[94 * 2];   // 95��° ���� x ��ǥ
	GLfloat lastY = vertices[94 * 2 + 1]; // 95��° ���� y ��ǥ

	// �� ��° ������ ������ ù ��° ������ ������ 95��° �� ������ �Ÿ���ŭ ���������� �̵�
	GLfloat originX = 2 * lastX; // 95��° ������ ���������� �̵��� ���ο� ���� x ��ǥ
	GLfloat originY = lastY;     // ���ο� ������ y ��ǥ�� �����ϰ� ����

	// �� ��° ���� (ū ���������� ���� ���������� ������ ����)
	radius = 0.02f + (94 * 0.003f); // �� ��° ������ ū ���������� ����
	for (int i = 95; i < numPoints; i++) {
		GLfloat angle = (i - 95) * angleIncrement;
		vertices[i * 2] = originX - (radius * cos(-angle));   // �� ������ �������� x ��ǥ ��� (�۾����� ����)
		vertices[i * 2 + 1] = originY - (radius * sin(-angle)); // �� ������ �������� y ��ǥ ���
		radius -= 0.003f; // ������ ������ ����
	}

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO ����ε�
	glBindVertexArray(0);
}
