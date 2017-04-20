#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 15) out;
layout (std140) uniform lut_edges {
	uint triTable[256*15];
};


out vec3 positionFeedback;


int case_to_poly[256] =
{
	0,1,1,2 ,1,2,2,3, 1,2,2,3, 2,3,3,2,
	1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,3,
	1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,3,
	2,3,3,2, 3,4,4,3, 3,4,4,3, 4,5,5,2,
	1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,3,
	2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,4,
	2,3,3,4, 3,4,2,3, 3,4,4,5, 4,5,3,2,
	3,4,4,3, 4,5,3,2, 4,5,5,4, 5,2,4,1,
	1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,3,
	2,3,3,4, 3,4,4,5, 3,2,4,3, 4,3,5,2,
	2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,4,
	3,4,4,3, 4,5,5,4, 4,3,5,2, 5,4,2,1,
	2,3,3,4, 3,4,4,5, 3,4,4,5, 2,3,3,2,
	3,4,4,5, 4,5,5,2, 4,3,5,4, 3,2,4,1,
	3,4,4,5, 4,5,3,4, 4,5,5,2, 3,4,2,1,
	2,3,3,2, 3,4,2,1, 3,2,4,1, 2,1,1,0
};


float isolevel = 0;

in VS_OUT
{
	vec4 f0123;
	vec4 f4567;
	uint mc_case;
	vec3 corners[8];
} vs_out[];

vec3 VertexInterp(float isolevel,vec3 p1, vec3 p2, float valp1, float valp2)
{
   float mu;
   vec3 p;

   if (abs(isolevel-valp1) < 0.00001)
      return(p1);
   if (abs(isolevel-valp2) < 0.00001)
      return(p2);
   if (abs(valp1-valp2) < 0.00001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

void main(){

	uint mccase = vs_out[0].mc_case;
	vec3 vertlist[12];

   vertlist[0]  = VertexInterp(isolevel,vs_out[0].corners[0],vs_out[0].corners[1],vs_out[0].f0123.x,vs_out[0].f0123.y);
   vertlist[1]  = VertexInterp(isolevel,vs_out[0].corners[1],vs_out[0].corners[2],vs_out[0].f0123.y,vs_out[0].f0123.z);
   vertlist[2]  = VertexInterp(isolevel,vs_out[0].corners[2],vs_out[0].corners[3],vs_out[0].f0123.z,vs_out[0].f0123.w);
   vertlist[3]  = VertexInterp(isolevel,vs_out[0].corners[3],vs_out[0].corners[0],vs_out[0].f0123.w,vs_out[0].f0123.x);
   vertlist[4]  = VertexInterp(isolevel,vs_out[0].corners[4],vs_out[0].corners[5],vs_out[0].f4567.x,vs_out[0].f4567.y);
   vertlist[5]  = VertexInterp(isolevel,vs_out[0].corners[5],vs_out[0].corners[6],vs_out[0].f4567.y,vs_out[0].f4567.z);

   vertlist[6]  = VertexInterp(isolevel,vs_out[0].corners[6],vs_out[0].corners[7],vs_out[0].f4567.z,vs_out[0].f4567.w);
   vertlist[7]  = VertexInterp(isolevel,vs_out[0].corners[7],vs_out[0].corners[4],vs_out[0].f4567.w,vs_out[0].f4567.x);
   vertlist[8]  = VertexInterp(isolevel,vs_out[0].corners[0],vs_out[0].corners[4],vs_out[0].f0123.x,vs_out[0].f4567.x);
   vertlist[9]  = VertexInterp(isolevel,vs_out[0].corners[1],vs_out[0].corners[5],vs_out[0].f0123.y,vs_out[0].f4567.y);
   vertlist[10] = VertexInterp(isolevel,vs_out[0].corners[2],vs_out[0].corners[6],vs_out[0].f0123.z,vs_out[0].f4567.z);
   vertlist[11] = VertexInterp(isolevel,vs_out[0].corners[3],vs_out[0].corners[7],vs_out[0].f0123.w,vs_out[0].f4567.w);

	/* Create the triangle */
	for (int i=0;i < case_to_poly[mccase];++i) {
		for(int o = 0; o < 3; ++o){
			positionFeedback = vertlist[triTable[mccase * 15 + 3 * i + o]];
			positionFeedback = positionFeedback.xzy;
			positionFeedback.x *= 95.f;
			positionFeedback.z *= 95.f;
			positionFeedback.y *= 255.f;
			EmitVertex();
		}
		EndPrimitive();
   }
}