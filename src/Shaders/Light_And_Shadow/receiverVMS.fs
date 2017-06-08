#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D normalMap;

uniform samplerCube depthMap;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float farPlane;
uniform float bumpFactor;

float specularStrength = 0.5f;
float ambientStrength = 0.1f;


float ShadowCalculation(vec3 fragPos)
{
	// get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPosition;
	vec2 moments = texture(depthMap,fragToLight).rg*farPlane;
	float distance = length(fragToLight);
	
	// Surface is fully lit. as the current fragment is before the light occluder
	if (distance <= moments.x)
		return 1.0;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x*moments.x);
	//variance = max(variance, 0.000002);
	variance = max(variance, 0.00002);

	float d = distance - moments.x;
	float p_max = variance / (variance + d*d);

	return p_max;
}

void main()
{
	//ambient light
	vec3 ambient = ambientStrength * lightColor;

	//diffuse light
	vec3 norm = texture(normalMap, TexCoord).rgb;
	norm = norm * 2.0 -1.0;
	norm.rg *= bumpFactor;
	norm = normalize(norm);

	vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular light
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	
	//vec3 result= (ambient + diffuse + specular) * vec3(1.f, 0.4f, 0.31f);
	vec3 result= (ambient + (ShadowCalculation(FragPos))*(diffuse + specular)) * vec3(1.f, 0.4f, 0.31f);

	color = texture(ourTexture, TexCoord) * vec4(result, 1.f);
	//color = vec4(vec3(ShadowCalculation(FragPos)/farPlane), 1.0);
}