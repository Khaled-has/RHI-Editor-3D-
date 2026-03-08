#ifndef VK_SHADER_H
#define VK_SHADER_H

#include <iostream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace GPU
{

	class VK_Shader
	{
	public:
		VK_Shader() {}
		~VK_Shader() {}

		void Create(const char* pVertex, const char* pFragment);

		inline const VkShaderModule& GetVertexShader() const { return pVS; }
		inline const VkShaderModule& GetFragmentShader() const { return pFS; }

	private:
		VkShaderModule pVS = VK_NULL_HANDLE;
		VkShaderModule pFS = VK_NULL_HANDLE;

		void CreateVertexShader(const char* pVertex);
		void CreateFragmentShader(const char* pFragment);
	};

}

#endif