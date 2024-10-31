#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#define MAX_LINE_LENGTH 512

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    unsigned int v1, v2, v3;
} Face;

typedef struct {
    Vertex* vertices;
    size_t vertex_count;
    Face* faces;
    size_t face_count;
} Model;

Model model;

void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

// read_obj_file �Լ� ���� ����� ����� �߰��մϴ�.
void read_obj_file(const char* filename, Model* model) {
    FILE* file;
    fopen_s(&file, filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    model->vertex_count = 0;
    model->face_count = 0;

    while (fgets(line, sizeof(line), file)) {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ')
            model->vertex_count++;
        else if (line[0] == 'f' && line[1] == ' ')
            model->face_count++;
    }

    printf("Vertices: %zu, Faces: %zu\n", model->vertex_count, model->face_count); // ����� ���

    fseek(file, 0, SEEK_SET);
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    model->faces = (Face*)malloc(model->face_count * sizeof(Face));

    size_t vertex_index = 0;
    size_t face_index = 0;

    while (fgets(line, sizeof(line), file)) {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ') {
            sscanf_s(line + 2, "%f %f %f", &model->vertices[vertex_index].x,
                &model->vertices[vertex_index].y,
                &model->vertices[vertex_index].z);
            printf("Vertex %zu: %f %f %f\n", vertex_index, model->vertices[vertex_index].x, model->vertices[vertex_index].y, model->vertices[vertex_index].z); // ����� ���
            vertex_index++;
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            unsigned int v1, v2, v3;
            // �� �����ø� �����ϴ� ���� "v1//vn1 v2//vn2 v3//vn3" ó��
            sscanf_s(line + 2, "%u//%*u %u//%*u %u//%*u", &v1, &v2, &v3);
            model->faces[face_index].v1 = v1 - 1; // OBJ �ε����� 1���� �����ϹǷ� 1�� ���ϴ�
            model->faces[face_index].v2 = v2 - 1;
            model->faces[face_index].v3 = v3 - 1;
            printf("Face %zu: %u %u %u\n", face_index, v1, v2, v3); // ����� ���
            face_index++;
        }
    }

    fclose(file);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // ī�޶� ��ġ ����
    gluLookAt(2.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // �ﰢ�� ���� ����
    glColor3f(1.0f, 0.0f, 0.0f); // ������

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < model.face_count; i++) {
        Face face = model.faces[i];
        Vertex v1 = model.vertices[face.v1];
        Vertex v2 = model.vertices[face.v2];
        Vertex v3 = model.vertices[face.v3];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();

    glutSwapBuffers();
}


void initOpenGL() {
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ����
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 100.0); // ���� ���� ����
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    read_obj_file("cube.obj", &model);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Cube");

    initOpenGL();
    glutDisplayFunc(display);
    glutMainLoop();

    free(model.vertices);
    free(model.faces);

    return 0;
}
