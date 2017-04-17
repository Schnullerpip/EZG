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











//#version 430 core

//layout(location = 0) in vec2 vertexPosition;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//out int instanceID;

//void main()
//{
//	instanceID = gl_InstanceID;
//	gl_Position = vec4(0.04f * vertexPosition.x -1, 0.04f *vertexPosition.y-1, gl_InstanceID, 1.0f);
//}