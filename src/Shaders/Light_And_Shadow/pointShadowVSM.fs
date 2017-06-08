#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

out vec4 color;

void main(){

	//distance between light source and the frag
	float lightDistance = length(FragPos.xyz - lightPos);

	color = vec4(lightDistance/farPlane, lightDistance*lightDistance/farPlane, 0.0f, 1.0f);
}
