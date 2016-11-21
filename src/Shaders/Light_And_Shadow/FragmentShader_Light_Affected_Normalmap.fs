#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D normalmap;

uniform samplerCube depthMap;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float farPlane;

float specularStrength = 0.5f;
float ambientStrength = 0.1f;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPosition;
    float currentDepth = length(fragToLight);

	float shadow = 0;
    float bias = 0.5;
	int samples = 20;
	float viewDistance = length(cameraPosition - fragPos);
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

	for(int i = 0; i < samples; ++i){
		float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= farPlane;
		if(currentDepth - bias > closestDepth){
			shadow += 1.0;
		}
	}
	shadow /= float(samples);
    return shadow;
}

void main()
{
	//ambient light
	vec3 ambient = ambientStrength * lightColor;

	//diffuse light
	//vec3 norm = normalize(Normal);
	vec3 norm = texture(normalmap, TexCoord).rgb;//opbatin normal from normalmap
	norm = normalize(norm * 2.0 - 1.0); //transform normal vector to range [-1, 1]

	vec3 lightDir = normalize(lightPosition - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular light
	vec3 viewDir = normalize(cameraPosition - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	
	//vec3 result= (ambient + diffuse + specular) * vec3(1.f, 0.4f, 0.31f);
	vec3 result= (ambient + (1.0-ShadowCalculation(FragPos))*(diffuse + specular)) * vec3(1.f, 0.4f, 0.31f);
	
	color = texture(ourTexture, TexCoord) * vec4(result, 1.f);
	//color = vec4(vec3(ShadowCalculation(FragPos)/farPlane), 1.0);
}
