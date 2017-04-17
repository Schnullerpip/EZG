#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

void build_house(vec4 position){
	gl_Position = position + vec4(-.2f, -.2f, 0, 0);
	EmitVertex();
	gl_Position = position + vec4(.2f, -.2f, 0, 0);
	EmitVertex();
	gl_Position = position + vec4(-.2f, .2f, 0, 0);
	EmitVertex();
	gl_Position = position + vec4(.2f, .2f, 0, 0);
	EmitVertex();
	gl_Position = position + vec4(0, .4f, 0, 0);
	EmitVertex();
    EndPrimitive();
}

void main() {    
	build_house(gl_in[0].gl_Position);
}  