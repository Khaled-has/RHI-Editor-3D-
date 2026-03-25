#include "VK_RenderPass.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{

	void VK_RenderPass::Destroy()
	{
	}

	void VK_RenderPass::Begin()
	{
		VK_Backend::Get()->GetDrawCmdsArray()->push_back(
			[&](VkCommandBuffer Cmd, uint32_t ImageIndex)
			{
				StartPass(Cmd, ImageIndex);
			}
		);
	}

	void VK_RenderPass::End()
	{
		VK_Backend::Get()->GetDrawCmdsArray()->push_back(
			[&](VkCommandBuffer Cmd, uint32_t ImageIndex)
			{
				EndPass(Cmd, ImageIndex);
			}
		);
	}

	void VK_RenderPass::StartPass(const VkCommandBuffer& pCmdBuf, uint32_t ImageIndex)
	{
		const bool IsDynamic = VK_Backend::Get()->GetDevice().GetSelectedDevice().pIsDynamicSupported;

		if (IsDynamic)
		{
			ImageMemBarrier(
				pCmdBuf, pImages[ImageIndex], VK_FORMAT_B8G8R8A8_UNORM,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1
			);

			VkClearValue clearColor = { .color{ 0.f, 0.f, 0.f, 1.f} };
			VkClearValue clearDepth = { .depthStencil{.depth = 1.0f, .stencil = 0} };

			BeginDynamicRendering(pCmdBuf, pImageViews[ImageIndex], ImageIndex, &clearColor, NULL, false);
		}
	}

	void VK_RenderPass::EndPass(const VkCommandBuffer& pCmdBuf, uint32_t ImageIndex)
	{
		const bool IsDynamic = VK_Backend::Get()->GetDevice().GetSelectedDevice().pIsDynamicSupported;

		if (IsDynamic)
		{
			vkCmdEndRendering(pCmdBuf);

			ImageMemBarrier(
				pCmdBuf, pImages[ImageIndex], VK_FORMAT_B8G8R8A8_UNORM,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1
			);
		}
	}

	void VK_RenderPass::Create(const RHI::GPU_RenderPassInfo pInfo)
	{
		pCreateInfo = pInfo;
		CreateImages();

		const bool pIsDynamic = VK_Backend::Get()->GetDevice().GetSelectedDevice().pIsDynamicSupported;

		if (!pIsDynamic)
		{
			CreateRenderPass();
			CreateFramebuffers();
		}
	}

	void VK_RenderPass::CreateImages()
	{
		const uint32_t NumImages = VK_Backend::Get()->GetSwapChain().GetImageCount();
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		// # Step 1: create the images
		pImages.resize(NumImages);
		pMemories.resize(NumImages);
		for (uint32_t i = 0; i < pImages.size(); i++)
		{
			VkFormat pFormat = VK_FORMAT_B8G8R8A8_UNORM;

			VkImageUsageFlags Usage = (VkImageUsageFlagBits)(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
			VkMemoryPropertyFlagBits PropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			VkImageCreateInfo ImageInfo = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.pNext = NULL,
				.flags = (VkImageCreateFlags)0,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = pFormat,
				.extent = VkExtent3D{ .width = 1440, .height = 720, .depth = 1 }, // Hard coded for now
				.mipLevels = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = Usage,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices = NULL,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
			};

			// # 1 Create the image object
			VkResult res = vkCreateImage(pDevice, &ImageInfo, NULL, &pImages[i]);
			VK_CHECK("vkCreateImage: VK_RenderPass", res);

			// # 2 Get the buffer memory requirements
			VkMemoryRequirements MemReqs = { 0 };
			vkGetImageMemoryRequirements(pDevice, pImages[i], &MemReqs);

			// # Step 3: get the memory type index
			uint32_t MemoryTypeIndex = GetMemoryTypeIndex(MemReqs.memoryTypeBits, PropertyFlags);

			// # Step 4: allocate memory
			VkMemoryAllocateInfo MemAllocInfo = {
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.pNext = NULL,
				.allocationSize = MemReqs.size,
				.memoryTypeIndex = MemoryTypeIndex
			};

			res = vkAllocateMemory(pDevice, &MemAllocInfo, NULL, &pMemories[i]);
			VK_CHECK("vkAllocateMemory", res);

			// # Step 5: bind memory
			res = vkBindImageMemory(pDevice, pImages[i], pMemories[i], 0);
			VK_CHECK("vkBindImageMemory", res);
		}

		// # Step 2: create image views
		pImageViews.resize(NumImages);
		for (uint32_t i = 0; i < pImageViews.size(); i++)
		{
			VkFormat Format = VK_FORMAT_B8G8R8A8_UNORM;
			VkImageAspectFlags AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT; // Hard coded for now
			pImageViews[i] = CreateImageView(pImages[i], pDevice, Format, AspectFlags);
		}

		// # Step 3: create samplers
		pSamplers.resize(NumImages);
		for (uint32_t i = 0; i < pSamplers.size(); i++)
		{
			VkFilter MinFilter = VK_FILTER_LINEAR;
			VkFilter MaxFilter = VK_FILTER_LINEAR;
			VkSamplerAddressMode AddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			pSamplers[i] = CreateTextureSampler(MinFilter, MaxFilter, AddressMode);
		}

		pFrameImage.pImages = pImages;
		pFrameImage.pViews = pImageViews;
		pFrameImage.pSamplers = pSamplers;
	}

	void VK_RenderPass::CreateFramebuffers()
	{

	}

	void VK_RenderPass::CreateRenderPass()
	{
	}

}