#version 330 core

layout(location = 0) in vec2 position; // 정점 좌표 입력
uniform vec2 translation; // 변환을 위한 uniform
uniform float scale;

void main()
{
    // translation을 적용하여 정점의 위치를 변환
    gl_Position = vec4(position + translation, 0.0, scale);
}
