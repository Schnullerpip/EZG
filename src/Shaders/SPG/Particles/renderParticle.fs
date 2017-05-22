#version 440 core

in float type;
in vec2 texCoords;

out vec4 color;

uniform sampler2D fireTex;

/*------TYPES-------
0 = particleFire
1 = particleSmoke
2 = particleBleh
3 = emitter
4 = nothing
-------------------*/

void main(){
	if(type == 0) //fire
	{
		color = texture(fireTex, texCoords);
	}
	else if(type == 1) //smoke
		color = vec4(0.1f, 0.1f, 0.1f, 1);
	else if(type == 2) //bleh
		color = vec4(0.3f, 1, 0.4f, 1);
	else if(type == 3)//emitter
		color = vec4(0.5f, 0.5f, 0.5f, 1);
	else
		color = vec4(0, 0, 0, 0);
}