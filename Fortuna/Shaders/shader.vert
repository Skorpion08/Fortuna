#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 2) in vec4 inColor;

uniform mat4 projection;

out vec4 color;

void main()
{
	gl_Position = projection * vec4(pos, 0.0, 1.0);
	color = inColor;
}