#version 330 core

layout(location = 0) in vec2 position;
uniform vec2 mousePos; // 마우스 위치를 uniform으로 전달받음

void main()
{
    // 마우스 위치를 기준으로 정점의 좌표를 변환
    gl_Position = vec4(position + mousePos, 0.0, 1.0);
}
