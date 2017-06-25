#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out VS_OUT {
	int instanceID;
}vs_out;

void main()
{
	gl_Position = vec4(2 * position.x, 2 * position.y, 2 * position.z ,1.0f);
	vs_out.instanceID = gl_InstanceID;
}