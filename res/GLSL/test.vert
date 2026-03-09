#version 460

struct VertexData
{
	float x, y, z;
};

layout (binding = 0) readonly buffer vertices { VertexData data[]; } in_vertices;

void main()
{
	VertexData vtx = in_vertices.data[gl_VertexIndex];

	gl_Position = vec4(vtx.x, vtx.y, vtx.z, 1.0);
}