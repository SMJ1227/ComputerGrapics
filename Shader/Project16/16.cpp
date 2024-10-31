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
GLuint spiralVao, spiralVbo;
//--- ���� ���� ����
GLfloat mouseX = 0.0f, mouseY = 0.0f;

mat4 view;
mat4 projection;

GLUquadricObj* qobj;

vec3 cameraPos = vec3(0.0f, 1.0f, 3.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����

int shape = 0;
int depte = 0;
int fill = 0;

int isSpiral = 0;
int spiralIndex = 0;
GLfloat spiralPoints[300];

int isMoving = 0;
GLfloat movingX = 0.0f;
GLfloat movingY = 0.0f;
GLfloat movingZ = 0.0f;
GLfloat KeymovingX = 0.0f;
GLfloat KeymovingY = 0.0f;
GLfloat KeymovingZ = 0.0f;

GLfloat rotationAngle = 0.0f;
int timerY = 0;
int jOrg = 0;	// ���� or ����
int changeRotate = 0;
int gxyz = 0;
int isScaling = 0;
GLfloat scaling = 0.0f;
int initialScaling = 0;
GLfloat initialX = 1.0f;
GLfloat initialY = 0.0f;
GLfloat initialZ = 0.0f;

void setupCamera() {
	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
}

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	if (isScaling == 1) {
		scaling += 0.01f;
		if (scaling > 0.5f) {
			isScaling = 2;
		}
	}
	else if (isScaling == 2) {
		scaling -= 0.01f;
		if (scaling < 0.0f) {
			isScaling = 1;
		}
	}

	if (initialScaling == 1) {
		scaling += 0.01f;
		if (scaling > 0.5f) {
			initialScaling = 2;
		}
	}
	else if (isScaling == 2 || initialScaling == 2) {
		scaling -= 0.01f;
		if (scaling < 0.0f) {
			initialScaling = 1;
		}
	}

	if (isSpiral) {
		// �����̷� ����Ʈ �迭���� ���� ��ǥ ��������
		movingX = spiralPoints[spiralIndex * 3];
		movingY = spiralPoints[spiralIndex * 3 + 1];
		movingZ = spiralPoints[spiralIndex * 3 + 2];

		spiralIndex++; // ���� ����Ʈ�� �̵�
		if (spiralIndex >= 100) spiralIndex = 0; // �����̷� �迭 ��ȯ
	}
	else {
		movingX += 0.05f;
		if (movingX > 2.0f) {
			movingX = 0;
			isMoving = 0;
		}
	}

	rotationAngle += radians(3.0f);
	if (changeRotate) {
		changeRotate++;
		if (changeRotate >= 60) {
			changeRotate = 0;
			timerY = 0;
			jOrg = 0;
		}
	}
	glutPostRedisplay();  // ȭ�� ������Ʈ ��û
	glutTimerFunc(100, timerFunc, 0);  // 0.1�ʸ��� Ÿ�̸� ��ȣ��
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
	switch (key) {
	case 'a':
		isScaling = 1;
		break;
	case 'd':
		initialScaling = 1;
		break;
	case '1': // Ű���� 1 : xz ��鿡 �����̷��� �׸���, �� �����̷� ��ġ�� ���� ��ü �̵� �ִϸ��̼�
		isSpiral = 1;
		break;
	case '2': //Ű���� 2 : �� ������ ������ ����ϸ� ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		isMoving = 1;
		movingX = 0;
		break;
	case '3': //Ű���� 3 : �� ������ �����ϸ鼭 ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		isMoving = 0;
		rotationAngle = 0;
		timerY = 0;
		jOrg = 1;
		changeRotate = 1;
		gxyz = 0;
		break;
	case '4': // Ű���� 4 : �� ������ �� ���� ����, �ٸ� ������ �Ʒ��� �̵��ϸ鼭 ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		isMoving = 0;
		rotationAngle = 0;
		timerY = 0;
		jOrg = 1;
		changeRotate = 1;
		gxyz = 1;
		break;
	case '5': // Ű���� 5 : �� ������ �� ���� Ȯ��, �ٸ� �� ���� ��ҵǸ� ������ �����ϱ�
		isMoving = 0; 
		timerY = 1;
		jOrg = 1;
		gxyz = 0;
		break;
	case 'i':
		cameraPos += vec3(0.0f, 0.5f, 0.0f);
		//cameraDirection += vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'j':
		cameraPos -= vec3(0.5f, 0.0f, 0.0f);
		//cameraDirection -= vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'k':
		cameraPos -= vec3(0.0f, 0.5f, 0.0f);
		//cameraDirection -= vec3(0.0f, 0.5f, 0.0f);
		setupCamera();
		break;
	case 'l':
		cameraPos += vec3(0.5f, 0.0f, 0.0f);
		//cameraDirection += vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'f':
		KeymovingX -= 0.1f;
		break;
	case 'g':
		KeymovingZ -= 0.1f;
		break;
	case 'h':
		KeymovingX += 0.1f;
		break;
	case 't':
		KeymovingZ += 0.1f;
		break;
	case 'r':
		KeymovingY += 0.1f;
		break;
	case 'y':
		KeymovingY -= 0.1f;
		break;
	case 's':
		initialScaling = 0;
		isScaling = 0;
		isMoving = 0;
		movingX = 0;
		rotationAngle = 0.0f;
		timerY = 0;
		jOrg = 0;
		gxyz = 0;
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

void initSpiralData() {
	const int numPoints = 100;  // �����̷��� ������ ���� ���� ����
	double deltaAngle = 0.2f;  // ���� �������� �����Ͽ� �� ���� ������ �����̷��� �׸�
	double deltaRadius = 0.05f;  // ������ ������ ����
	double angle = 0.0f, radius = 0.0f;

	for (int i = 0; i < numPoints; ++i) {
		spiralPoints[i * 3] = radius * cos(angle);   // X
		spiralPoints[i * 3 + 1] = 0;                 // Y
		spiralPoints[i * 3 + 2] = radius * sin(angle); // Z
		angle += deltaAngle;
		radius += deltaRadius;
	}

	glGenVertexArrays(1, &spiralVao);
	glBindVertexArray(spiralVao);

	glGenBuffers(1, &spiralVbo);
	glBindBuffer(GL_ARRAY_BUFFER, spiralVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spiralPoints), spiralPoints, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO ����ε�
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

	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram();
	Initbuffer();

	glEnable(GL_DEPTH_TEST);
	initSpiralData();

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard); // Ű �Է� �ݹ�
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
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
GLvoid drawScene() {
	//--- ���� ���� �� ���� Ŭ����
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);
	// �׸���
	if (isSpiral) {
		glBindVertexArray(spiralVao);
		glDrawArrays(GL_LINE_STRIP, 0, 100);
		glBindVertexArray(0); // VAO ����ε�
	}
	// VAO ���ε�
	glBindVertexArray(vao);

	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 1.0f, 0.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	mat4 model = mat4(1.0f);
	if (jOrg == 0) {
		if (isMoving) {
			model = translate(model, vec3(1.0f - movingX, 0.0f, 0.0f));
		}
		else if (isSpiral) {
			model = translate(model, vec3(movingX, movingY, movingZ));
		}
		else {
			model = translate(model, vec3(1.0f, 0.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (timerY == 1) {
			model = rotate(model, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
			model = translate(model, vec3(1.0f, 0.0f, 0.0f));		
			model = scale(model, vec3(0.5, 0.5, 0.5));
		}
		if (timerY == 0) {
			if (!gxyz) {
				float r = 1.0f; // ���� ������
				float x = r * cos(rotationAngle);
				float z = -r * sin(rotationAngle);
				model = translate(model, vec3(x, 0.0f, z));
			}
			if (gxyz) {
				float r = 1.0f; // ���� ������
				float x = r * cos(rotationAngle);
				float y = -r * sin(rotationAngle);
				model = translate(model, vec3(x, y, 0.0f));
			}
		}
	}
	if (isScaling) {
		model = scale(model, vec3(scaling, scaling, scaling));
	}
	if (initialScaling) {
		model = translate(model, vec3(-initialX, -initialY, -initialZ));
		model = scale(model, vec3(scaling, scaling, scaling));
		model = translate(model, vec3(initialX, initialY, initialZ));
	}
	model = translate(model, vec3(KeymovingX, KeymovingY, KeymovingZ));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	mat4 model2 = mat4(1.0f);
	if (jOrg == 0) {
		if (isMoving) {
			model2 = translate(model2, vec3(-1.0f + movingX, 0.0f, 0.0f));
		}
		else if (isSpiral) {
			model2 = translate(model2, vec3(movingX, movingY, movingZ));
		}
		else {
			model2 = translate(model2, vec3(-1.0f, 0.0f, 0.0f));
		}
	}
	else if (jOrg == 1) {
		if (timerY == 1) {
			model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
			model2 = translate(model2, vec3(-1.0f, 0.0f, 0.0f));
			model2 = scale(model2, vec3(2.0, 2.0, 2.0));
		}
		if (timerY == 0) {
			if (!gxyz) {
				float r = 1.0f; // ���� ������
				float initialAngle = 3.14159265;
				float x = r * cos(rotationAngle + initialAngle);
				float z = -r * sin(rotationAngle + initialAngle);
				model2 = translate(model2, vec3(x, 0.0f, z));
			}
			if (gxyz) {
				float r = 1.0f; // ���� ������
				float initialAngle = 3.14159265;
				float x = r * cos(rotationAngle + initialAngle);
				float y = -r * sin(rotationAngle + initialAngle);
				model2 = translate(model2, vec3(x, y, 0.0f));
			}
		}
	}
	if (isScaling) {
		model2 = scale(model2, vec3(scaling, scaling, scaling));
	}
	model2 = translate(model2, vec3(KeymovingX, KeymovingY, KeymovingZ));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
	qobj = gluNewQuadric(); // ��ü �����ϱ�
	gluQuadricDrawStyle(qobj, GLU_LINE); // ���� ��Ÿ��
	gluQuadricNormals(qobj, GLU_SMOOTH); // ���� ����
	gluQuadricOrientation(qobj, GLU_OUTSIDE); // ���� ����
	gluSphere(qobj, 0.5, 50, 50); // ��ü �����

	// ���� ��
	mat4 xyzModel = mat4(1.0f); // ȸ�� ����
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(xyzModel));

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(48 * sizeof(GLuint))); // X�� �׸���
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(50 * sizeof(GLuint))); // Y�� �׸���
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(52 * sizeof(GLuint))); // Z�� �׸���

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
	GLfloat vertices[] = {
		// ��ǥ             // ���� (R, G, B)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // ���� �Ʒ� ���� (����)
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // ���� �Ʒ� ������ (�ʷ�)
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // ���� �� ������ (�Ķ�)
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // ���� �� ���� (���)
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, // ���� �Ʒ� ���� (��ũ)
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // ���� �Ʒ� ������ (�ϴû�)
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, // ���� �� ������ (���)
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  // ���� �� ���� (ȸ��)
		 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f,	// �簢�� ������
		 // X��� Y���� ���� ��ǥ (�߰�)
		-1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // X��
		 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // X��
		 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Y��
		 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Y��
		 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Z��
		 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Z��
	};

	// �ε��� ������
	GLuint indices[] = {
		// ���� (1�� ��)
		3, 2, 6, 3, 6, 7,
		// ���� (2�� ��)
		0, 3, 7, 0, 7, 4,
		// ���� (3�� ��)
		1, 2, 6, 1, 6, 5,
		// ���� (4�� ��)
		0, 1, 5, 0, 5, 4,
		// ���� (5�� ��)
		4, 5, 6, 4, 6, 7,
		// �ٴڸ� (6�� ��)
		0, 1, 2, 0, 2, 3,
		// ���� 1
		0, 1, 8,
		// ���� 2
		0, 4, 8,
		// ���� 3
		4, 5, 8,
		// ���� 4
		1, 5, 8,
		// X��
		9, 10,
		// Y��
		11, 12,
		// Z��
		13, 14
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

	// ��ġ �Ӽ� ���� (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// ���� �Ӽ� ���� (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// VAO ����ε�
	glBindVertexArray(0);
}
