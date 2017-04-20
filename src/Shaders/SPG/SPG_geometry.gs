#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	int instanceID;
} vs_data[3];

out GS_OUT {
	vec4 fragPos;
	flat int layer;
} gs_out;

void main() {    
	gl_Layer = vs_data[0].instanceID;

	for(int i = 0; i < 3; ++i){
		gl_Position = gl_in[i].gl_Position;
		gs_out.fragPos = gl_Position;
		gs_out.layer = gl_Layer;
		EmitVertex();
	}
	EndPrimitive();
}