#version 330 core

layout (location = 0) in vec3 p;

out vec3 position;

void main()
{
	position = p;
}