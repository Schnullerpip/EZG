#version 430 core

in GS_OUT {
	vec4 fragPos;
	flat int layer;
}gs_in;

uniform vec2 pillars[3];

out vec4 color;

void main()
{

	vec2 pills[3] = pillars;

	//rotate the pillars
	for(int i = 0; i < 2; ++i){
		pills[i] = vec2(sin(gs_in.layer*0.2f * i)*0.8f, cos(gs_in.layer*0.2f + i)*0.8f);
	}

	float f = 0;

	//add pillars
	for(int i = 0; i < 3; ++i){
		f += 1 / (length(gs_in.fragPos.xy - pills[i].xy)) - 1;
	}

	//add center negative (water flow)
	f -= 1 / length(gs_in.fragPos.xy) - 1;

	//add strong negative values at outer edge keeps solid rocks in bounds
	f = f - pow(length(gs_in.fragPos), 5);

	//add helix
	vec2 vec = vec2(cos(gs_in.layer), sin(gs_in.layer));
	f += dot(vec, gs_in.fragPos.xy);

	//add shelves
	f += cos(gs_in.fragPos.y);

	//float mappedLayer = gs_in.layer/255.f;
    color = vec4(f, 0, 0, 1);
}

//#version 430 core

//layout(location = 0) out vec4 color;

//in vec4 vertexPosition;

//float heightBase = 0.5f;
//float heightModifyer = 0.04f;

//flat in int instance;

//uniform sampler3D noiseTex;

//void main()
//{
//	//x / y --> -1 to 1 Z --> 0 to 50

//	float noise = texture(noiseTex, vec3(0.5f * vertexPosition.x  + 1, 0.5f * vertexPosition.y  + 1, 0.02f * vertexPosition.z)).x;

//	float f = -noise;									//0.5

//	//Make sure theres negative at the sides
//	//if(vertexPosition.x > 0.9 || vertexPosition.x < -0.9 || vertexPosition.y > 0.9 ||  vertexPosition.y < -0.9)
//		f = f - 9999;

//	//Make sure we have ground everywhere
//	//if(vertexPosition.z < 1)
//		f = f + 9999;

//	//Reduce by height, but add base					//+ 0.5 - 0.02 * z
//	f = f - ((vertexPosition.z - 5) * heightModifyer);

//	//Add Piller to mid									//+ 0.5 - distance to mid
//	f = f + 1.0f * (0.5 - ((vertexPosition.x * vertexPosition.x) + (vertexPosition.y * vertexPosition.y)));

//	//f = clamp( f * 99999,0,1 );
//	color = vec4(f,0,0,1.0);
//}