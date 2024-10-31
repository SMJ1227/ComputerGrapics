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
GLfloat mouseX = 0.0f, mouseY = 0.0f;

mat4 view;
mat4 projection;

int drawingFace = 1;	// �׸� ���� ��
int whatFace1 = 0;		// �׸� ���� ����
int whatFace2 = 0;
int shape = 0;

vec3 cameraPos = vec3(0.0f, 2.0f, 3.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����

void setupCamera() {
	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
}

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	glutPostRedisplay();  // ȭ�� ������Ʈ ��û
	glutTimerFunc(100, timerFunc, 0);  // 0.1�ʸ��� Ÿ�̸� ��ȣ��
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
	switch (key) {
	case 'i':
		cameraPos += vec3(0.0f, 0.5f, 0.0f);
		cameraDirection += vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'j':
		cameraPos -= vec3(0.5f, 0.0f, 0.0f);
		cameraDirection -= vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'k':
		cameraPos -= vec3(0.0f, 0.5f, 0.0f);
		cameraDirection -= vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'l':
		cameraPos += vec3(0.5f, 0.0f, 0.0f);
		cameraDirection += vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case '1':
		shape = 0;
		whatFace1 = 1;
		break;
	case '2':
		shape = 0;
		whatFace1 = 2;
		break;
	case '3':
		shape = 0;
		whatFace1 = 3;
		break;
	case '4':
		shape = 0;
		whatFace1 = 4;
		break;
	case '5':
		shape = 0;
		whatFace1 = 5;
		break;
	case '6':
		shape = 0;
		whatFace1 = 6;
		break;
	case '7':
		shape = 1;
		whatFace1 = 1;
		break;
	case '8':
		shape = 1;
		whatFace1 = 2;
		break;
	case '9':
		shape = 1;
		whatFace1 = 3;
		break;
	case '0':
		shape = 1;
		whatFace1 = 4;
		break;
	case 'c':
	case 't':
		if (drawingFace == 1) drawingFace = 2;
		else drawingFace = 1;
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
	glutPostRedisplay(); // ȭ�� ���� ��û
}

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(NULL);
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);

	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram();
	Initbuffer();

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard); // Ű �Է� �ݹ�
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	//glutTimerFunc(1000, timerFunc, 0);
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
GLvoid drawScene() {
	//--- ���� ���� �� ���� Ŭ����
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);
	// VAO ���ε�
	glBindVertexArray(vao);
	//�׸���
	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 1.0f, 0.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	mat4 model = mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	if (shape == 0) {	// ����ü
		if (drawingFace == 1) {
			if (whatFace1 >= 1 && whatFace1 <= 6) {
				GLvoid* offset = (GLvoid*)((whatFace1 - 1) * 6 * sizeof(GLuint));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset);
			}
			else {
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}
		else {
			whatFace1 = rand() % 6 + 1;
			whatFace2 = rand() % 6 + 1;
			GLvoid* offset1 = (GLvoid*)((whatFace1 - 1) * 6 * sizeof(GLuint));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset1);
			GLvoid* offset2 = (GLvoid*)((whatFace2 - 1) * 6 * sizeof(GLuint));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, offset2);
		}
	}
	else if (shape == 1) {  // ���ü
		if (drawingFace == 1) {
			if (whatFace1 >= 1 && whatFace1 <= 6) {
				GLvoid* offset = (GLvoid*)((36 + (whatFace1 - 1) * 3) * sizeof(GLuint));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, offset);
			}
			else {
				glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)(36 * sizeof(GLuint)));
			}
		}
		else {
			whatFace1 = rand() % 4 + 1;
			whatFace2 = rand() % 4 + 1;
			GLvoid* offset1 = (GLvoid*)((36 + (whatFace1 - 1) * 3) * sizeof(GLuint));
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, offset1);
			GLvoid* offset2 = (GLvoid*)((36 + (whatFace2 - 1) * 3) * sizeof(GLuint));
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, offset2);
		}		
	}

	
	// VAO ����ε�
	glBindVertexArray(0);
	//--- ���� ����
	glutSwapBuffers();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}


void Initbuffer() {
	setupCamera();
	// ���� ������: �ִ� 4���� ���� ����
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, // ���� �Ʒ� ����
		 0.5f, -0.5f, -0.5f, // ���� �Ʒ� ������
		 0.5f,  0.5f, -0.5f, // ���� �� ������
		-0.5f,  0.5f, -0.5f, // ���� �� ����
		-0.5f, -0.5f,  0.5f, // ���� �Ʒ� ����
		 0.5f, -0.5f,  0.5f, // ���� �Ʒ� ������
		 0.5f,  0.5f,  0.5f, // ���� �� ������
		-0.5f,  0.5f,  0.5f  // ���� �� ����
	};

	// �ε��� ������
	GLuint indices[] = {
		// ���� (1�� ��)
		3, 2, 6,
		3, 6, 7,
		// ���� (2�� ��)
		0, 3, 7,
		0, 7, 4,
		// ���� (3�� ��)
		1, 2, 6,
		1, 6, 5,
		// ���� (4�� ��)
		0, 1, 5,
		0, 5, 4,
		// ���� (5�� ��)
		4, 5, 6,
		4, 6, 7,
		// �ٴڸ� (6�� ��)
		0, 1, 2,
		0, 2, 3,
		// ���ü 1
		6, 4, 3,
		// ���ü 2
		4, 2, 3,
		// ���ü 3
		3, 2, 6,
		// ���ü 4
		6, 2, 4
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO ����ε�
	glBindVertexArray(0);
}
