#version 440 core

layout (location = 0) in vec3 pos;
layout (location = 1) in float typ;
layout (location = 2) in float lif;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
	vec3 position;
	float type;
	float life;
}vs_out;

void main()
{
	vs_out.position = pos;
	vs_out.type = typ;
	vs_out.life = lif;

    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
}
