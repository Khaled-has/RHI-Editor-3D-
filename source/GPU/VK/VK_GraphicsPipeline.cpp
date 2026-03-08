#include "VK_GraphicsPipeline.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{

	void VK_GraphicsPipeline::Create(const std::vector<VK_PipelineBinding>* pBindingsInfo)
	{
		CreateDescriptorPool(pBindingsInfo);
		CreateDescriptorLayout(pBindingsInfo);
		AllocateDescriptorSets();
		UpdateDescriptorSets(pBindingsInfo);
	}

	void VK_GraphicsPipeline::Destroy()
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		vkFreeDescriptorSets(pDevice, pDescriptorPool, (uint32_t)pDescriptorSets.size(), pDescriptorSets.data());
		vkDestroyDescriptorSetLayout(pDevice, pDescriptorSetLayout, NULL);
		vkDestroyDescriptorPool(pDevice, pDescriptorPool, NULL);

		vkDestroyPipeline(pDevice, pPipeline, NULL);
		vkDestroyPipelineLayout(pDevice, pPipelineLayout, NULL);
	}

	void VK_GraphicsPipeline::Bind(uint32_t ImageIndex)
	{
		const VkCommandBuffer& CmdBuf = VK_Backend::Get()->GetCmdBuf(ImageIndex);

		// 1 # Bind pipeline
		vkCmdBindPipeline(
			CmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline
		);

		// 2 # Bind descriptor sets
		vkCmdBindDescriptorSets(
			CmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipelineLayout,
			0, 1,
			&pDescriptorSets[ImageIndex], 0, NULL
		);
	}

	void VK_GraphicsPipeline::CreateDescriptorPool(const std::vector<VK_PipelineBinding>* pBindingsInfo)
	{
		const uint32_t NumImages = VK_Backend::Get()->GetSwapChain().GetImageCount();
		std::vector<VkDescriptorPoolSize> PoolSizes;

		for (uint32_t i = 0; i < pBindingsInfo->size(); i++)
		{
			PoolSizes.push_back(
				VkDescriptorPoolSize{
					.type = pBindingsInfo->at(i).pDescType,
					.descriptorCount = NumImages
				}
			);
		}

		VkDescriptorPoolCreateInfo PoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = 0,
			.maxSets = NumImages,
			.poolSizeCount = (uint32_t)PoolSizes.size(),
			.pPoolSizes = PoolSizes.data()
		};

		VkResult res = vkCreateDescriptorPool(
			VK_Backend::Get()->GetDevice().GetDevice(),
			&PoolInfo, NULL, &pDescriptorPool
		);
		VK_CHECK("vkCreateDescriptorPool", res);
		
	}

	void VK_GraphicsPipeline::CreateDescriptorLayout(const std::vector<VK_PipelineBinding>* pBindingsInfo)
	{
		std::vector<VkDescriptorSetLayoutBinding> Bindings;

		for (uint32_t i = 0; i < pBindingsInfo->size(); i++)
		{
			Bindings.push_back(
				VkDescriptorSetLayoutBinding{
					.binding = pBindingsInfo->at(i).pBinding,
					.descriptorType = pBindingsInfo->at(i).pDescType,
					.descriptorCount = 1,
					.stageFlags = pBindingsInfo->at(i).pStageFlag
				}
			);
		}

		VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.bindingCount = (uint32_t)Bindings.size(),
			.pBindings = Bindings.data()
		};

		VkResult res = vkCreateDescriptorSetLayout(
			VK_Backend::Get()->GetDevice().GetDevice(),
			&CreateInfo, NULL, &pDescriptorSetLayout
		);
		VK_CHECK("vkCreateDescriptorSetLayout", res);
	}

	void VK_GraphicsPipeline::AllocateDescriptorSets()
	{
		const uint32_t NumImages = VK_Backend::Get()->GetSwapChain().GetImageCount();
		std::vector<VkDescriptorSetLayout> Layouts(NumImages, pDescriptorSetLayout);

		VkDescriptorSetAllocateInfo AllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = NULL,
			.descriptorPool = pDescriptorPool,
			.descriptorSetCount = NumImages,
			.pSetLayouts = Layouts.data()
		};

		pDescriptorSets.resize(NumImages);

		VkResult res = vkAllocateDescriptorSets(
			VK_Backend::Get()->GetDevice().GetDevice(),
			&AllocateInfo, pDescriptorSets.data()
		);
	}

	void VK_GraphicsPipeline::UpdateDescriptorSets(const std::vector<VK_PipelineBinding>* pBindingsInfo)
	{
		std::vector<VkWriteDescriptorSet> WriteDescriptorSets;

		for (uint32_t i = 0; i < VK_Backend::Get()->GetSwapChain().GetImageCount(); i++)
		{
			for (uint32_t j = 0; j < pBindingsInfo->size(); j++)
			{
				// # Buffer
				if (pBindingsInfo->at(j).pBindingType == VK_BINDING_BUFFER_INFO)
				{
					VkDescriptorBufferInfo BufferInfo = {

					};

					WriteDescriptorSets.push_back(
						VkWriteDescriptorSet{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = pDescriptorSets[i],
							.dstBinding = pBindingsInfo->at(j).pBinding,
							.dstArrayElement = 0,
							.descriptorCount = 1,
							.descriptorType = pBindingsInfo->at(j).pDescType,
							.pBufferInfo = &BufferInfo
					});
				}
				// # Image
				else if (pBindingsInfo->at(j).pBindingType == VK_BINDING_IMAGE_INFO)
				{
					VkDescriptorImageInfo ImageInfo = {
						//.sampler = pBindingsInfo->at(j).pTexture,
						//.imageView = pBindingsInfo->at(j).pTexture,
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					};

					WriteDescriptorSets.push_back(
						VkWriteDescriptorSet{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = pDescriptorSets[i],
							.dstBinding = pBindingsInfo->at(j).pBinding,
							.dstArrayElement = 0,
							.descriptorCount = 1,
							.descriptorType = pBindingsInfo->at(j).pDescType,
							.pImageInfo = &ImageInfo
					});
				}
				// # Uniform buffer
				else if (pBindingsInfo->at(j).pBindingType == VK_BINDING_UNIFORM_INFO)
				{
					/*for (uint32_t u = 0; u < pBindingsInfo->at(j).pUniformbuffer->size(); u++)
					{
						VkDescriptorBufferInfo UniformInfo = {

						};

						WriteDescriptorSets.push_back(
							VkWriteDescriptorSet{
								.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
								.dstSet = pDescriptorSets[i],
								.dstBinding = pBindingsInfo->at(j).pBinding,
								.dstArrayElement = 0,
								.descriptorCount = 1,
								.descriptorType = pBindingsInfo->at(j).pDescType,
								.pBufferInfo = &UniformInfo
							}
						);
					}*/
				}
			}
		}

		vkUpdateDescriptorSets(
			VK_Backend::Get()->GetDevice().GetDevice(), (uint32_t)WriteDescriptorSets.size(),
			WriteDescriptorSets.data(), 0, NULL
		);
	}

}