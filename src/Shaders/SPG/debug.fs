#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler3D ourTexture;
uniform int layer;

void main()
{
    color = texture(ourTexture, vec3(TexCoord.xy, layer/255.0));
}
