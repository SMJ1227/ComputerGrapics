#version 330 core

layout (location = 0) in vec2 position;

void main()
{
    // ���콺 ��ġ�� �������� ������ ��ǥ�� ��ȯ
    gl_Position = vec4(translation, 0.0, 1.0);
}
