#version 330 core

layout(location = 0) in vec2 position; // ���� ��ǥ �Է�
uniform vec2 translation; // ��ȯ�� ���� uniform
uniform float rotation; // ȸ���� ���� uniform

void main()
{
    // ȸ�� ��ȯ ��� ����
    mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation),
                               sin(rotation), cos(rotation));
    vec2 rotatedPosition = rotationMatrix * position;

    // translation�� �����Ͽ� ������ ��ġ�� ��ȯ
    gl_Position = vec4(rotatedPosition + translation, 0.0, 1.0);
}
