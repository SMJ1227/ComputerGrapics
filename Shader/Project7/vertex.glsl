#version 330 core

layout(location = 0) in vec2 position;
uniform vec2 mousePos; // ���콺 ��ġ�� uniform���� ���޹���

void main()
{
    // ���콺 ��ġ�� �������� ������ ��ǥ�� ��ȯ
    gl_Position = vec4(position + mousePos, 0.0, 1.0);
}
