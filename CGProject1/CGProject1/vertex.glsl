#version 330 core

layout (location = 0) in vec2 position;

void main()
{
    // 마우스 위치를 기준으로 정점의 좌표를 변환
    gl_Position = vec4(translation, 0.0, 1.0);
}
