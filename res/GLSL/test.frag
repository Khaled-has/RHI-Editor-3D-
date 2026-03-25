#version 460

layout (location = 0) out vec4 out_Color;

layout (location = 0) in vec2 aColor;

layout (binding = 2) uniform sampler2D texture_2d;

void main()
{
	vec4 color = texture(texture_2d, aColor);
	out_Color = color;
}