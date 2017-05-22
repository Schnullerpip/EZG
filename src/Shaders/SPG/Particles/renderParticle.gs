#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

in VS_OUT {
	vec3 position;
	float type;
	float life;
}gs_in[];

/*------TYPES-------
0 = particleFire -> move/change it
1 = particleSmoke -> move/change it
2 = particleBleh -> move/kill it
3 = emitter -> create other particles
4 = nothing -> discard
-------------------*/

out float type; //type
out float life;
out vec2 texCoords;

float scaleFactor = 1;

void main(){

	type = gs_in[0].type;
	life = gs_in[0].life;
	if(type == 0)//fire
	{
	scaleFactor += 0;
	}
	else if(type == 1)//smoke
	{
		scaleFactor += gs_in[0].life/10;
	}
	float doubleScale = 2*scaleFactor;
	gl_Position = gl_in[0].gl_Position;


	//depending on type create a cube or a quad
	//wheather or not Particle or Emitter we need the front quad

	//lower left
		gl_Position.x -= scaleFactor;
		gl_Position.y -= scaleFactor;
		texCoords = vec2(0, 0);
		EmitVertex();
	//lower rght
		gl_Position.x += doubleScale;
		texCoords = vec2(1, 0);
		EmitVertex();
	//upper left
		gl_Position.x -= doubleScale;
		gl_Position.y += doubleScale;
		texCoords = vec2(0, 1); 
		EmitVertex();
	//uper right
		gl_Position.x += doubleScale;
		texCoords = vec2(1, 1); 
		EmitVertex();

	EndPrimitive();
}