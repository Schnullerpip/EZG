#version 330 core

layout (points) in;
layout (points, max_vertices = 10) out;

out vec3 positionFeedback;
out float typeFeedback;
out float lifeFeedback;

in vec3 position[];
in float type[];
in float life[];

void main(){

	positionFeedback = position[0];
	positionFeedback.y += 1;
	positionFeedback.z += 1;

	typeFeedback = 666;
	lifeFeedback = 777;

	EmitVertex();
	EndPrimitive();
}