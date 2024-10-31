#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 in_Color; //--- �÷� ����: attribute position 1
out vec3 out_Color; //--- �����׸�Ʈ ���̴����� ����

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    out_Color = in_Color;
}