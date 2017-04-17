//#version 410 core
//layout(points) in;
//layout(triangle_strip, max_vertices = 15) out;

//layout(std140) uniform lut_edges {
//	uint triTable[256*15];
//};

//uniform sampler3D densityMap;

//in VS_OUT
//{
//	vec4 f0123;
//	vec4 f4567;
//	uint mc_case;
//	vec3 corners[8];
//} data[];


//out vec4 originalVertexPosition;

//out vec3 positionFeedback;


//int numOfPolys[256] = int[256] (

//								0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,2,
//								1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,3,
//								1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,3,
//								2,3,3,2,3,4,4,3,3,4,4,3,4,5,5,2,
//								1,2,2,3,2,3,3,4,3,3,3,4,3,4,4,3,
//								2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,4,
//								2,3,3,4,3,4,2,3,3,4,4,5,4,5,3,2,
//								3,4,4,3,4,5,3,2,4,5,5,4,5,2,4,1,
//								1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,3,
//								2,3,3,4,3,4,4,5,3,2,4,3,4,3,5,2,
//								2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,4,
//								3,4,4,3,4,5,5,4,4,3,5,2,5,4,2,1,
//								2,3,3,4,3,4,4,5,3,4,4,5,2,3,3,2,
//								3,4,4,5,4,5,5,2,4,3,5,4,3,2,4,1,
//								3,4,4,5,4,5,3,4,4,5,5,2,3,4,2,1,
//								2,3,3,2,3,4,2,1,3,3,4,1,2,1,1,0
//);


//float edgeTransforms[36] = float [36] (
//									   0.5, 0, 1,
//									   1, 0, 0.5,
//									   0.5, 0, 0,
//									   0, 0, 0.5,
//									   
//									   0.5, 1, 1,
//									   1, 1, 0.5,
//									   0.5, 1, 0,
//									   0, 1, 0.5,	
//									   
//									   0, 0.5, 1,
//									   1, 0.5, 1,
//									   1, 0.5, 0,	
//									   0, 0.5, 0
//);

//vec3 interpolateVertex(in float iso, in vec3 vec0, in float dens0, in vec3 vec1, in float dens1)
//{
//	float interpolation_value = (iso - dens0) / (dens1 - dens0);
//	return mix(vec0, vec1, interpolation_value);
//}

//void main()
//{
//	originalVertexPosition = gl_in[0].gl_Position;
//	
//	int numPolygons = numOfPolys[data[0].mc_case];

//	
//	vec3 v1 = interpolateVertex(0, data[0].corners[0], data[0].f0123.x, data[0].corners[1], data[0].f0123.y);

//	
//	for(int polygon = 0; polygon < numPolygons; polygon++)
//	{
//		for(int vertex = 0; vertex < 3; vertex++)
//		{
//			//int edge = triTable[int(mc_case) * 15 + 3 * polygon + vertex] *3;
//			uint edge = triTable[int(data[0].mc_case) * 15 + 3 * polygon + vertex] *3;
//			if(edge != 99)
//			{
//				positionFeedback = vec3(originalVertexPosition.x + edgeTransforms[edge], originalVertexPosition.y + edgeTransforms[edge +1], originalVertexPosition.z + edgeTransforms[edge +2]);
//				EmitVertex();
//			}
//		}
//		EndPrimitive();
//	}
//}








#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 15) out;
layout (std140) uniform lut_edges {
	uint triTable[256*16];
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















//int edgeTable[256]=
//{
//	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
//	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
//	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
//	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
//	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
//	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
//	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
//	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
//	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
//	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
//	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
//	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
//	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
//	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
//	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
//	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
//	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
//	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
//	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
//	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
//	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
//	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
//	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
//	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
//	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
//	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
//	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
//	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
//	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
//	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
//	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
//	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
//};
