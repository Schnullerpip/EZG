#version 330 core

layout (location = 0) in vec3 pos;

out vec3 position;

void main()
{
	position = pos;
}