#version 440 core

layout (location = 0) in vec3 pos;
layout (location = 1) in float typ;
layout (location = 2) in float lif;

out vec3 position;
out float type;
out float life;
out int seed;

void main()
{
	seed = gl_VertexID;
	position = pos;
	type = typ;
	life = lif;
}