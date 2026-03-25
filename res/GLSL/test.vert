#version 460

layout (location = 0) out vec2 aColor;

vec3 colors[3] = {
	vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)
};

struct VertexData
{
	float x, y, z;
	float u, v;
};

layout (binding = 0) readonly buffer vertices { VertexData data[]; } in_vertices;
layout (binding = 1) uniform UniformBuffer 
{ 
	mat4 model; 
	mat4 VP;
} ubo;

void main()
{
	VertexData vtx = in_vertices.data[gl_VertexIndex];
	aColor = vec2(vtx.u, vtx.v);
	gl_Position = ubo.VP * ubo.model * vec4(vtx.x, vtx.y, vtx.z, 1.0);
}