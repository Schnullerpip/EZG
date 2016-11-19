#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main(){

	//distance between light source and the frag
	float lightDistance = length(FragPos.xyz - lightPos);

	//map to [0, 1]
	lightDistance = lightDistance/farPlane;

	gl_FragDepth = lightDistance;
}