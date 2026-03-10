#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include <stdio.h>
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>

namespace GPU
{

	class VK_SwapChain
	{
	public:
		VK_SwapChain() {}
		~VK_SwapChain() {}

		void Create();
		void Destroy();

		inline const VkSwapchainKHR& GetSwapchain() const { return pSwapChain; }

		inline uint32_t GetImageCount() const { return (uint32_t)pImages.size(); }

		inline const VkImage& GetImage(uint32_t Index) const { return pImages[Index]; }
		inline const VkImageView& GetImageView(uint32_t Index) const { return pImageViews[Index]; }

		inline const VkFramebuffer& GetFramebuffer(uint32_t Index) const { return pFramebuffers[Index]; }

		inline VkSurfaceFormatKHR GetSurfaceFormat() const { return pSwChainSurfaceFormat; }

		inline const VkRenderPass& GetRenderPass() const { return pRenderPass; }

	private:
		VkSwapchainKHR pSwapChain = VK_NULL_HANDLE;
		VkRenderPass pRenderPass  = VK_NULL_HANDLE;

		std::vector<VkFramebuffer> pFramebuffers;

		std::vector<VkImage> pImages;
		std::vector<VkImageView> pImageViews;

		VkSurfaceFormatKHR pSwChainSurfaceFormat{};

		void CreateRenderPass();
		void CreateFramebuffers();
	};

}

#endif