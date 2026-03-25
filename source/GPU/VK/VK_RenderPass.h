#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "GPU/GPU_RenderPass.h"

#include <vulkan/vulkan.h>

namespace GPU
{

	struct VK_FrameImage
	{
		std::vector<VkImage> pImages;
		std::vector<VkImageView> pViews;
		std::vector<VkSampler> pSamplers;
	};

	class VK_RenderPass : public RHI::GPU_RenderPass
	{
	public:
		VK_RenderPass() {}
		VK_RenderPass(const RHI::GPU_RenderPassInfo pInfo)
		{
			Create(pInfo);
		}
		~VK_RenderPass() {}

		virtual void Destroy() override;

		virtual void Begin() override;
		virtual void End() override;

		void StartPass(const VkCommandBuffer& pCmdBuf, uint32_t ImageIndex);
		void EndPass(const VkCommandBuffer& pCmdBuf, uint32_t ImageIndex);

		inline const VK_FrameImage& GetFrameImage() const { return pFrameImage; }

	private:
		RHI::GPU_RenderPassInfo pCreateInfo;

		std::vector<VkFramebuffer> pFramebuffers;

		std::vector<VkImage> pImages;
		std::vector<VkSampler> pSamplers;
		std::vector<VkImageView> pImageViews;
		std::vector<VkDeviceMemory> pMemories;

		VK_FrameImage pFrameImage;

		virtual void Create(const RHI::GPU_RenderPassInfo pInfo) override;

		void CreateImages();

		void CreateFramebuffers();
		void CreateRenderPass();
	};

}

#endif