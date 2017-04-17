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



//#version 430 core

//layout(points) in;
//layout(points, max_vertices = 50) out;
//in int instanceID[];
//flat out int instance;
//out vec4 vertexPosition;

//void main()
//{
//    gl_Position = vec4(gl_in[0].gl_Position.xy,0,1.0f);
//	gl_Layer = instanceID[0];
//	instance = instanceID[0];
//	vertexPosition = vec4(gl_in[0].gl_Position.xy,instanceID[0],1.0f);
//	EmitVertex();
//	EndPrimitive();
//}