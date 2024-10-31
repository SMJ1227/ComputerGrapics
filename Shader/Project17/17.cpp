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

vec3 cameraPos = vec3(2.0f, 2.0f, 2.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����

bool shape = false;
bool depth = true;

GLfloat translationY = 0.0f;
GLfloat rotationAngle = 0.0f;
GLfloat scaling = 1.0f;
int isScaling = 0;

bool pressedY = false;
bool pressedT = false;
bool pressedF = false;
bool pressedS = false;
bool pressedB = false;
bool pressedO = false;
bool pressedR = false;

void setupCamera() {
	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
}

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	if (pressedT || pressedY || pressedR) {
		rotationAngle += radians(3.0f);
	}
	if (pressedF && rotationAngle > radians(90.0f)) {
		rotationAngle = 0.0f;
		pressedF = !pressedF;
	}
	if (pressedS) {
		translationY += 0.05f;
		if (translationY > 1.0f) {
			translationY = 0.0f;
			pressedS = !pressedS;
		}
	}
	if (pressedB) {
		if (isScaling == 1) {
			scaling += 0.05f;
			if (scaling > 1.0f) {
				isScaling = 2;
			}
		}
		else if (isScaling == 2) {
			scaling -= 0.05f;
			if (scaling < 0.0f) {
				isScaling = 1;
			}
		}
	}
	if (pressedO) {
		rotationAngle += radians(3.0f);
		if (rotationAngle > radians(216.0f)) {
			pressedO = !pressedO;
		}
	}
	glutPostRedisplay();  // ȭ�� ������Ʈ ��û
	glutTimerFunc(100, timerFunc, 0);  // 0.1�ʸ��� Ÿ�̸� ��ȣ��
}

void keyboard(unsigned char key, int x, int y) {
	printf("Key pressed: %c\n", key); // ����� �޽��� ���
	switch (key) {
	case '0':
		shape = !shape;
		break;
	case 'h':
		if (depth) {
			glDisable(GL_DEPTH_TEST);
			depth = !depth;
		}
		else {
			glEnable(GL_DEPTH_TEST);
			depth = !depth;
		}
		break;
	case 'y':
		pressedY = !pressedY;
		break;
	case 't':
		rotationAngle = 0.0f;
		pressedT = !pressedT;
		break;
	case 'f':
		rotationAngle = 0.0f;
		pressedF = !pressedF;
		break;
	case 's':
		translationY = 0.0f;
		pressedS = !pressedS;
		break;
	case 'b':
		isScaling = 2;
		scaling = 1.0f;
		pressedB = !pressedB;
		break;
	case 'o':
		rotationAngle = 0.0f;
		pressedO = !pressedO;
		break;
	case 'r':
		rotationAngle = 0.0f;
		pressedR = !pressedR;
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

	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram();
	Initbuffer();

	glEnable(GL_DEPTH_TEST);

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
	
	// VAO ���ε�
	glBindVertexArray(vao);

	glUniform3f(glGetUniformLocation(shaderProgramID, "color"), 0.0f, 1.0f, 0.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	if (!shape) {
		mat4 model0 = mat4(1.0f);
		mat4 model1 = mat4(1.0f);
		mat4 model2 = mat4(1.0f);
		mat4 model3 = mat4(1.0f);
		mat4 model4 = mat4(1.0f);
		mat4 model5 = mat4(1.0f);

		for (int i = 0; i < 6; i++) {
			switch (i)
			{
			case 0:	// ����
				if (pressedT) {
					model0 = translate(model0, vec3(0.0f, 0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
					model0 = rotate(model0, rotationAngle, vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
					model0 = translate(model0, vec3(0.0f, -0.5f, 0.0f)); // �������� ����
				}
				if (pressedY) {
					model0 = rotate(model0, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model0));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			case 1:	// �� ����
				if (pressedB) {
					model1 = scale(model1, vec3(scaling, scaling, scaling));
				}
				if (pressedY) {
					model1 = rotate(model1, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model1));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			case 2:	// �� ������
				if (pressedF) {
					model2 = translate(model2, vec3(0.5f, -0.5f, 0.0f));
					model2 = rotate(model2, rotationAngle, vec3(0.0f, 0.0f, -1.0f));
					model2 = translate(model2, vec3(-0.5f, 0.5f, 0.0f));
				}
				if (pressedY) {
					model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			case 3:	// �Ʒ���
				if (pressedY) {
					model3 = rotate(model3, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model3));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			case 4:	// �� ����
				if (pressedS) {
					model4 = translate(model4, vec3(0.0f, translationY, 0.0f));
				}
				if (pressedY) {
					model4 = rotate(model4, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model4));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			case 5: // �� ������
				if (pressedS) {
					model5 = translate(model5, vec3(0.0f, translationY, 0.0f));
				}
				if (pressedY) {
					model5 = rotate(model5, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model5));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i * 6) * sizeof(GLuint)));
				break;
			}
		}
	}
	else if (shape) {
		mat4 model0 = mat4(1.0f);
		mat4 model1 = mat4(1.0f);
		mat4 model2 = mat4(1.0f);
		mat4 model3 = mat4(1.0f);
		mat4 model4 = mat4(1.0f);

		for (int i = 0; i < 6; i++) {
			switch (i)
			{
			case 0:	// 	�� ������	
				if (pressedO) {
					model0 = translate(model0, vec3(0.0f, -0.5f, -0.5f)); // ȸ���� ��ġ�� �̵�
					model0 = rotate(model0, -rotationAngle, vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
					model0 = translate(model0, vec3(0.0f, 0.5f, 0.5f)); // �������� ����
				}
				if (pressedY) {
					model0 = rotate(model0, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				if (pressedR) {
					if(rotationAngle < radians(100.0f)) {
						model0 = translate(model0, vec3(0.0f, -0.5f, -0.5f)); // ȸ���� ��ġ�� �̵�
						model0 = rotate(model0, -rotationAngle, vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
						model0 = translate(model0, vec3(0.0f, 0.5f, 0.5f)); // �������� ����
					}
					else {
						model0 = translate(model0, vec3(0.0f, -0.5f, -0.5f)); // ȸ���� ��ġ�� �̵�
						model0 = rotate(model0, -radians(100.0f), vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
						model0 = translate(model0, vec3(0.0f, 0.5f, 0.5f)); // �������� ����
					}
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model0));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((36 + (i * 3)) * sizeof(GLuint)));
				break;
			case 1: // �� ����
				if (pressedO) {
					model1 = translate(model1, vec3(-0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
					model1 = rotate(model1, rotationAngle, vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
					model1 = translate(model1, vec3(0.5f, 0.5f, 0.0f)); // �������� ����
				}
				if (pressedY) {
					model1 = rotate(model1, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				if (pressedR) {
					if (rotationAngle > radians(100.0f) && rotationAngle < radians(200.0f)) {
						model1 = translate(model1, vec3(-0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
						model1 = rotate(model1, rotationAngle - radians(100.0f), vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
						model1 = translate(model1, vec3(0.5f, 0.5f, 0.0f)); // �������� ����
					}
					else if(rotationAngle > radians(200.0f)){
						model1 = translate(model1, vec3(-0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
						model1 = rotate(model1, radians(100.0f), vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
						model1 = translate(model1, vec3(0.5f, 0.5f, 0.0f)); // �������� ����
					}
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model1));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((36 + (i * 3)) * sizeof(GLuint)));
				break;
			case 2:	// �� ����
				if (pressedO) {
					model2 = translate(model2, vec3(0.0f, -0.5f, 0.5f)); // ȸ���� ��ġ�� �̵�
					model2 = rotate(model2, rotationAngle, vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
					model2 = translate(model2, vec3(0.0f, 0.5f, -0.5f)); // �������� ����
				}
				if (pressedY) {
					model2 = rotate(model2, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				if (pressedR) {
					if (rotationAngle > radians(200.0f) && rotationAngle < radians(300.0f)) {
						model2 = translate(model2, vec3(0.0f, -0.5f, 0.5f)); // ȸ���� ��ġ�� �̵�
						model2 = rotate(model2, rotationAngle - radians(200.0f), vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
						model2 = translate(model2, vec3(0.0f, 0.5f, -0.5f)); // �������� ����
					}
					else if (rotationAngle > radians(300.0f)) {
						model2 = translate(model2, vec3(0.0f, -0.5f, 0.5f)); // ȸ���� ��ġ�� �̵�
						model2 = rotate(model2, radians(100.0f), vec3(1.0f, 0.0f, 0.0f)); // ȸ�� ����
						model2 = translate(model2, vec3(0.0f, 0.5f, -0.5f)); // �������� ����
					}
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model2));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((36 + (i * 3)) * sizeof(GLuint)));
				break;
			case 3:	// �� ������
				if (pressedO) {
					model3 = translate(model3, vec3(0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
					model3 = rotate(model3, -rotationAngle, vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
					model3 = translate(model3, vec3(-0.5f, 0.5f, 0.0f)); // �������� ����
				}
				if (pressedY) {
					model3 = rotate(model3, rotationAngle, vec3(0.0f, 1.0f, 0.0f)); // ȸ�� ����
				}
				if (pressedR) {
					if (rotationAngle > radians(300.0f) && rotationAngle < radians(400.0f)) {
						model3 = translate(model3, vec3(0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
						model3 = rotate(model3, -rotationAngle + radians(300.0f), vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
						model3 = translate(model3, vec3(-0.5f, 0.5f, 0.0f)); // �������� ����
					}
					else if (rotationAngle > radians(400.0f)) {
						model3 = translate(model3, vec3(0.5f, -0.5f, 0.0f)); // ȸ���� ��ġ�� �̵�
						model3 = rotate(model3, -radians(100.0f), vec3(0.0f, 0.0f, 1.0f)); // ȸ�� ����
						model3 = translate(model3, vec3(-0.5f, 0.5f, 0.0f)); // �������� ����
					}
				}
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model3));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((36 + (i * 3)) * sizeof(GLuint)));
				break;
			case 4:	// �Ʒ���
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model4));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));
				break;
			}
		}
	}

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
