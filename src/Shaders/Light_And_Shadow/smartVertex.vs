#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

//normal map specific
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

out vec3 lightPos;
out vec3 camPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoord = texCoord;
	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(position, 1.f));

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix *tangent);
	vec3 B = normalize(normalMatrix *bitangent);
	vec3 N = normalize(normalMatrix *normal);

	mat3 TBN = transpose(mat3(T, B, N));
	TangentLightPos = TBN * lightPosition;
	TangentViewPos = TBN * cameraPosition;
	TangentFragPos = TBN * FragPos;
}
