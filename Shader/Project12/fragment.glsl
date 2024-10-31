#version 330 core

out vec4 FragColor;
uniform vec3 color; // 도형의 색상

void main()
{
    FragColor = vec4(color, 1.0);
}
