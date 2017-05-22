#version 440 core

layout (points) in;
layout (points, max_vertices = 1000) out;

layout (xfb_buffer = 0) out vec3 positionFeedback;
layout (xfb_buffer = 1) out float typeFeedback;
layout (xfb_buffer = 2) out float lifeFeedback;

in vec3 position[];
in float type[];
in float life[];
in int seed[];

float fireLife = 10;
float smokeLife = 200;
float blehLife = 100;

float fireSpeed = 0.1f;
float smokeSpeed = 0.2;
float blehSpeed = 0.01;

uniform vec3 random;

float rand(vec2 co){
    return 2*(fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453))-1.0f;
}

void main(){

	positionFeedback = position[0];

	typeFeedback = type[0];

	lifeFeedback = life[0];

	//EmitVertex();
	//EndPrimitive();

	/*------TYPES-------
	0 = particleFire -> move/change it
	1 = particleSmoke -> move/change it
	2 = particleBleh -> move/kill it
	3 = emitter -> create other particles
	4 = nothing -> discard
	-------------------*/




	/*-------------------PARTICLES-----------------------*/

	vec2 s = vec2(seed[0] * random.x, seed[0] * random.y);

	if(type[0] == 0){//particleFire -> move/change it
		//move up
		positionFeedback.y += fract(fireSpeed + rand(s));
		positionFeedback.x += rand(s);
		positionFeedback.z += rand(s)*random.z;

		//reduce lifetime
		lifeFeedback = life[0]+1;
		if(lifeFeedback >= fireLife){
			typeFeedback = 1;//turn to smoke
			lifeFeedback = 0;
		}
	}
	else if(type[0] == 1){//particleSmoke
		//move up
		positionFeedback.y += smokeSpeed;

		//reduce lifetime
		lifeFeedback = life[0]+1;
		if(lifeFeedback >= smokeLife){
			typeFeedback = 2;//turn to bleh
			lifeFeedback = 0;
		}
	}
	else if(type[0] == 2){//particleBleh
		if(lifeFeedback < blehLife){
			//move
			positionFeedback.x += blehSpeed;

			//reduce lifetime
			lifeFeedback = life[0]+1;
			if(lifeFeedback >= blehLife){
				typeFeedback = 4;//turn to nothing
			}
		}
	}



	/*--------------------EMITTER------------------------*/



	else if(type[0] == 3){//emitter
		//keep emitter

		//create 10 particles per emitter
		for(int i = 0; i < 10; ++i){
			EmitVertex();
			typeFeedback = 0;
			lifeFeedback = 0;
		}
	}

	EmitVertex();
	EndPrimitive();
}