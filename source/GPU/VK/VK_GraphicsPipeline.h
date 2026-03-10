#ifndef VK_GRAPHICSPIPELINE_H
#define VK_GRAPHICSPIPELINE_H

#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

#include "VK_Buffer.h"

namespace GPU
{
	// # Type of binding attribute
	enum VK_BindingInfoType
	{
		VK_BINDING_UNDEFINE     = 0,
		VK_BINDING_BUFFER_INFO  = 1,
		VK_BINDING_IMAGE_INFO   = 2,
		VK_BINDING_UNIFORM_INFO = 3
	};

	// # The of bindings for descriptor sets
	struct VK_PipelineBinding
	{
		uint32_t pBinding;
		VkDescriptorType pDescType;
		VkShaderStageFlags pStageFlag;
		VK_BindingInfoType pBindingType;
		VK_Buffer* pBuffer;
		//VK_Texture* pTexture = NULL;
		std::vector<VK_Buffer>* pUniformBuffers = NULL;
	};

	// # The pipeline it self
	class VK_GraphicsPipeline
	{
	public:
		VK_GraphicsPipeline() {}
		~VK_GraphicsPipeline() {}

		void Create(
			const std::vector<VK_PipelineBinding>* pBindingsInfo, 
			const VkShaderModule& pVs, const VkShaderModule& pFs
		);
		void Destroy();

		void Bind(uint32_t ImageIndex);

	private:
		VkPipeline pPipeline					   = VK_NULL_HANDLE;
		VkPipelineLayout pPipelineLayout		   = VK_NULL_HANDLE;

		std::vector<VkDescriptorSet> pDescriptorSets;
		VkDescriptorSetLayout pDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool pDescriptorPool		   = VK_NULL_HANDLE;

		void CreateDescriptorPool(const std::vector<VK_PipelineBinding>* pBindingsInfo);
		void CreateDescriptorLayout(const std::vector<VK_PipelineBinding>* pBindingsInfo);
		void AllocateDescriptorSets();
		void UpdateDescriptorSets(const std::vector<VK_PipelineBinding>* pBindingsInfo);

		void CreatePipeline(const VkShaderModule& pVs, const VkShaderModule& pFs);
	};

}

#endif