#version 410 core

layout(location = 0) in vec2 ws;

out VS_OUT
{
	vec4 f0123;
	vec4 f4567;
	uint mc_case;
	vec3 corners[8];
} vs_out;

uniform sampler3D tex3d;

void main()
{
	gl_Position = vec4(ws.x, gl_InstanceID,ws.y, 1.0f);
	
	vec3 step = vec3(1.f/95.f, 1.f/95.f, 1.f/255.f);
	
	//ws to uv
	vec3 uv = vec3(ws.x * step.x, ws.y * step.y, gl_InstanceID * step.z);

	vs_out.corners[0] = vec3(uv.x,			uv.y + step.y,	uv.z);
	vs_out.corners[1] = vec3(uv.x + step.x,	uv.y + step.y,	uv.z);
	vs_out.corners[2] = vec3(uv.x + step.x,	uv.y,			uv.z);
	vs_out.corners[3] = vec3(uv.x,			uv.y,			uv.z);
	vs_out.corners[4] = vec3(uv.x,			uv.y + step.y,	uv.z + step.z);
	vs_out.corners[5] = vec3(uv.x + step.x,	uv.y + step.y,	uv.z + step.z);
	vs_out.corners[6] = vec3(uv.x + step.x,	uv.y,			uv.z + step.z);
	vs_out.corners[7] = vec3(uv.x,			uv.y,			uv.z + step.z);

	//calculate MarchingCube Case
	vs_out.f0123 = vec4( texture(tex3d, vs_out.corners[0]).x,
						 texture(tex3d, vs_out.corners[1]).x,
						 texture(tex3d, vs_out.corners[2]).x,
						 texture(tex3d, vs_out.corners[3]).x );
	
	vs_out.f4567 = vec4( texture(tex3d, vs_out.corners[4]).x,
						 texture(tex3d, vs_out.corners[5]).x,
						 texture(tex3d, vs_out.corners[6]).x,
						 texture(tex3d, vs_out.corners[7]).x );
	
	// determine which of the 256 marching cubes cases for this cell:
	ivec4 n0123 = ivec4(clamp( vs_out.f0123 * 99999,0,1 ));
	ivec4 n4567 = ivec4(clamp( vs_out.f4567 * 99999,0,1 ));
	vs_out.mc_case = uint(  (n0123.x     ) | (n4567.x << 4)
						| (n0123.y << 1) | (n4567.y << 5)
						| (n0123.z << 2) | (n4567.z << 6)
						| (n0123.w << 3) | (n4567.w << 7));
}