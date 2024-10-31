#version 330 core

layout(location = 0) in vec2 position; // 정점 좌표 입력
uniform vec2 translation; // 변환을 위한 uniform
uniform float rotation; // 회전을 위한 uniform

void main()
{
    // 회전 변환 행렬 적용
    mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation),
                               sin(rotation), cos(rotation));
    vec2 rotatedPosition = rotationMatrix * position;

    // translation을 적용하여 정점의 위치를 변환
    gl_Position = vec4(rotatedPosition + translation, 0.0, 1.0);
}
