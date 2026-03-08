#include "VK_Shader.h"

#include <fstream>

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{

	std::vector<char> ReadFile(const std::string pFilename)
	{
		std::ifstream pFile(pFilename, std::ios::ate | std::ios::binary);

		size_t pFileSize = (size_t)pFile.tellg();
		std::vector<char> buffer(pFileSize);

		pFile.seekg(0);
		pFile.read(buffer.data(), pFileSize);

		pFile.close();
		return buffer;
	}

	VkShaderModule CreateShaderModuleFromBinary(std::string pFileName)
	{
		std::string pFinalPath = std::string(RES_PATH) + "Shaders/" + "spv/" + pFileName;
		auto pCode = ReadFile(pFinalPath);

		VkShaderModuleCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = pCode.size(),
			.pCode = reinterpret_cast<const uint32_t*>(pCode.data())
		};

		VkShaderModule pShaderModule = VK_NULL_HANDLE;
		VkResult res = vkCreateShaderModule(
			VK_Backend::Get()->GetDevice().GetDevice(), &CreateInfo, 
			NULL, &pShaderModule
		);
		VK_CHECK("vkCreateShaderModule", res);

		return pShaderModule;
	}

	void VK_Shader::Create(const char* pVertex, const char* pFragment)
	{
		CreateVertexShader(pVertex);
		CreateFragmentShader(pFragment);
	}

	void VK_Shader::CreateVertexShader(const char* pVertex)
	{
		pVS = CreateShaderModuleFromBinary(pVertex);
	}

	void VK_Shader::CreateFragmentShader(const char* pFragment)
	{
		pFS = CreateShaderModuleFromBinary(pFragment);
	}

}