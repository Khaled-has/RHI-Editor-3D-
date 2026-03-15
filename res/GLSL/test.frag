#version 460

layout (location = 0) out vec4 out_Color;

layout (location = 0) in vec3 aColor;

//layout (binding = 2) uniform sampler2D texture_2d;

void main()
{
	out_Color = vec4(aColor, 1.0);
}