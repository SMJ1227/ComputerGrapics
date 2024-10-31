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
	{0.5f, 0.5f, 0.5f},  // ��ũ
	{1.0f, 1.0f, 0.0f}   // ���
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

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	// Y ��ǥ�� �����ϴٰ� ���� �� �̻��̸� ����
	if (increasing) {
		for (int i = 0; i < 3; i++) {
			pointX[i] += 0.02f;
			pointY[i] += 0.02f;  // Y ��ǥ ����
			if (pointY[i] >= maxpointY[i]) {  // Y ��ǥ�� �ִ밪�� �������� ��
				pointX[i] = originpointX[i];
				pointY[i] = originpointY[i];
				matrix[i].shapeType = (matrix[i].shapeType - 1) % 4 + 2;
			}
		}
	}

	glutPostRedisplay();  // ȭ�� ������Ʈ ��û
	glutTimerFunc(100, timerFunc, 0);  // 0.1�ʸ��� Ÿ�̸� ��ȣ��
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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
	matrix[0].x = 0.5f; matrix[0].y = 0.5f;  // 1��и�
	matrix[0].shapeType = 2;
	matrix[1].x = -0.5f;  matrix[1].y = 0.5f;  // 2��и�
	matrix[1].shapeType = 3;
	matrix[2].x = -0.5f; matrix[2].y = -0.5f; // 3��и�
	matrix[2].shapeType = 4;
	matrix[3].x = 0.5f;  matrix[3].y = -0.5f; // 4��и�
	matrix[3].shapeType = 5;
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
	glutTimerFunc(1000, timerFunc, 0);
	glutMainLoop();
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
}

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

	// ���ڼ� �׸���
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f);
	glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), 0.0f, 0.0f);  // ���ڼ��� �߾ӿ� �׸���
	glDrawArrays(GL_LINES, 4, 2);  // ���� �� �׸���
	glDrawArrays(GL_LINES, 6, 2);  // ���� �� �׸���

	for (int i = 0; i < 4; i++) {
		// �� ��и鿡 �´� �� ����
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), colors[i].r, colors[i].g, colors[i].b);

		switch (matrix[i].shapeType) {
		case 2: // ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_LINES, 0, 2);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // ������ ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			
			break;
		case 3: // �ﰢ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // ������ ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x - (pointX[matrix[i].shapeType - 2]), matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			break;
		case 4: // �簢��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f);  // ������ ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x - 0.1f, matrix[i].y + pointY[matrix[i].shapeType - 2]);
			glDrawArrays(GL_POINTS, 8, 1);
			break;
		case 5: // ������
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 5); 
			break;	
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
		-0.1f, -0.1f, // ���� �Ʒ�
		0.1f, -0.1f,  // ������ �Ʒ�
		0.1f,  0.1f,  // ������ ��
		-0.1f,  0.1f,  // ���� ��
		-1.0f,  0.0f, // ���� �� ���� ��
		1.0f,  0.0f,  // ���� �� ������ ��
		0.0f,  1.0f,  // ���� �� ��� ��
		0.0f, -1.0f,   // ���� �� �ϴ� ��
		0.1f,  0.0f,  // ù ��° ��
		0.03f, 0.09f, // �� ��° ��
		-0.08f, 0.06f, // �� ��° ��
		-0.08f, -0.06f, // �� ��° ��
		0.03f, -0.09f  // �ټ� ��° ��
	};

	// �ε��� ������
	GLuint indices[] = {
		0, 1, 2,  // ù ��° �ﰢ��
		0, 2, 3,   // �� ��° �ﰢ��
		4, 5,     // ���� ��
		6, 7      // ���� ��
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
