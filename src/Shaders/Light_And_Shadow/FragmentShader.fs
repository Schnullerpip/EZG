#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 lightColor;


void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	
    color = texture(ourTexture, TexCoord);
	//color = ambient * vec3(1.f, 0.4f, 0.31f);
}