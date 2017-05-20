#version 330 core

layout (points) in;
layout (points, max_vertices = 10) out;

out vec3 positionFeedback;

in vec3 position[];

void main(){

	positionFeedback = position[0];
	positionFeedback.x += 1;
	EmitVertex();
	EndPrimitive();
}