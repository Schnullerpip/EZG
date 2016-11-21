#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

out vec3 TangentLightpos;
out vec3 TangentViewpos;
out vec3 TangentFragpos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	mat3 TBN = mat3(T, B, N);

	TangentLightPos = TBN * lightpos;
	TangentViewPos = TBN * viewPos;
	TangentFragPos = TBN * FragPos;

    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoord = texCoord;
	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(position, 1.f));
}
