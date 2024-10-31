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

//--- ���� ���� ����
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
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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
	printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
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

	// ���콺 ��ġ uniform ���� ��ġ ���
	mousePosLocation = glGetUniformLocation(shaderProgramID, "mousePos");
}

//--- ��� �ݹ� �Լ�
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID);

	// VAO ���ε�
	glBindVertexArray(vao);

	// ���� �׸���
	for (int i = 0; i < numShapes; i++) {
		// ���� ����
		if (shapes[i].shapeType == 1) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 1.0f); // �Ķ�
		}
		else if (shapes[i].shapeType == 2) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 0.0f, 0.0f); // ����
		}
		else if (shapes[i].shapeType == 3) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 0.0f, 0.0f); // ����
		}
		else if (shapes[i].shapeType == 4) {
			glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 1.0f, 1.0f, 0.0f); // ���
		}

		// ���콺 ��ġ�� ���� ��ȯ ����
		if (shapes[i].shapeType == 1 ) {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x + 0.1f, shapes[i].y + 0.1f);
		}
		else if (shapes[i].shapeType == 2) {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x, shapes[i].y + 0.1f);
		}
		// �ﰢ���� �簢���� ���콺 ��ġ�� ���� ��ġ�� ���ؼ� �׸���
		else {
			glUniform2f(glGetUniformLocation(shaderProgramID, "mousePos"), shapes[i].x, shapes[i].y);
		}

		// ���� �׸���
		switch (shapes[i].shapeType) {
		case 1: // ��
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 2: // ��
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 3: // �ﰢ��
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 4: // �簢��
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // �簢�� �׸���
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
		-0.1f,  0.1f  // ���� ��
	};

	// �ε��� ������
	GLuint indices[] = {
		0, 1, 2,  // ù ��° �ﰢ��
		0, 2, 3   // �� ��° �ﰢ��
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


