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

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	for (int i = 0; i < matrixs; i++) {
		switch (matrix[i].moveMode)
		{
		case 0:
			// �ﰢ�� ��ġ ������Ʈ
			matrix[i].x += matrix[i].directionX * speed;
			matrix[i].y += matrix[i].directionY * speed;

			// �� �浹 ó�� - X��
			if (matrix[i].x >= 1.0f) {
				matrix[i].directionX = -1; // X ���� ����
				matrix[i].x = 1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}
			else if (matrix[i].x <= -1.0f) {
				matrix[i].directionX = 1; // X ���� ����
				matrix[i].x = -1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}

			// �� �浹 ó�� - Y��
			if (matrix[i].y >= 1.0f) {
				matrix[i].directionY = -1; // Y ���� ����
				matrix[i].y = 1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}
			else if (matrix[i].y <= -1.0f) {
				matrix[i].directionY = 1; // Y ���� ����
				matrix[i].y = -1.0f; // �� ������ ������ �ʵ��� ��ġ ����
			}

			// ���� ����: �̵� ���⿡ ���� ����
			if (matrix[i].directionX == 1 && matrix[i].directionY == 1) {
				angle[i] = 45.0f;  // ������ �� ����
			}
			else if (matrix[i].directionX == -1 && matrix[i].directionY == 1) {
				angle[i] = 135.0f; // ���� �� ����
			}
			else if (matrix[i].directionX == -1 && matrix[i].directionY == -1) {
				angle[i] = 225.0f; // ���� �Ʒ� ����
			}
			else if (matrix[i].directionX == 1 && matrix[i].directionY == -1) {
				angle[i] = 315.0f; // ������ �Ʒ� ����
			}

			break;
		case 1:
			// ��ġ ������Ʈ
			matrix[i].x += matrix[i].directionX * speed;

			// �� �浹 ó��
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

			// ȭ�� �Ʒ��ʿ� �����ϸ� ������ ���� ��ȯ
			if (matrix[i].y <= -1.0f) {
				matrix[i].directionY = 1; // �������� �̵�
			}
			// ȭ�� ���ʿ� �����ϸ� ������ �Ʒ��� ��ȯ
			else if (matrix[i].y >= 1.0f) {
				matrix[i].directionY = -1; // �Ʒ������� �̵�
			}
			break;
		default:
			break;
		}
	}

	glutPostRedisplay();  // ȭ�� ������Ʈ ��û
	glutTimerFunc(100, timerFunc, 0);  // 0.1�ʸ��� Ÿ�̸� ��ȣ��
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
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
		// �׻� index1�� �� ���� �ε����� ����Ű���� ��
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
	//printf("x = %d, y = %d\n", x, y); // ����� �޽��� ���
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
				// �巡�� ����
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

		glutPostRedisplay(); // ȭ�� ���� ��û
	}
}

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(NULL);
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
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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

	for (int i = 0; i < matrixs; i++) {
		// �� ��и鿡 �´� �� ����
		int color = rand() % 4;
		glUniform3f(glGetUniformLocation(shaderProgramID, "color"), matrix[i].r, matrix[i].g, matrix[i].b);

		switch (matrix[i].shapeType) {
		case 1:
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x + 0.1f, matrix[i].y + 0.1f);
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 2: // ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y + 0.1f);
			glDrawArrays(GL_LINES, 0, 2);

			break;
		case 3: // �ﰢ��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 4: // �簢��
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			break;
		case 5: // ������
			glUniform2f(glGetUniformLocation(shaderProgramID, "translation"), matrix[i].x, matrix[i].y);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 5);
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
