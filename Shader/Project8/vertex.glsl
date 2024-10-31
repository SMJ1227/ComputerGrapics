#version 330 core

layout(location = 0) in vec2 position; // ���� ��ǥ �Է�
uniform vec2 translation; // ��ȯ�� ���� uniform
uniform float scale;

void main()
{
    // translation�� �����Ͽ� ������ ��ġ�� ��ȯ
    gl_Position = vec4(position + translation, 0.0, scale);
}
