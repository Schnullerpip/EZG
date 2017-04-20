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