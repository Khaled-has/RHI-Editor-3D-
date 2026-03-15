#include "VK_wrappar.h"

#include "VK_Backend.h"

namespace GPU
{
	uint32_t GetMemoryTypeIndex(uint32_t MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
	{
		const VkPhysicalDeviceMemoryProperties& MemProps = VK_Backend::Get()->GetDevice().GetSelectedDevice().m_memProps;

		for (uint32_t i = 0; i < MemProps.memoryTypeCount; i++)
		{
			const VkMemoryType& MemType = MemProps.memoryTypes[i];
			uint32_t CurBitmask = (1 << i);
			bool IsCurMemTypeSupported = (MemTypeBitsMask & CurBitmask);
			bool HasRequiredMemProps = ((MemType.propertyFlags & ReqMemPropFlags) == ReqMemPropFlags);

			if (IsCurMemTypeSupported && HasRequiredMemProps)
			{
				return i;
			}
		}

		VK_LOG_ERROR("Cannot find memory type for type {0} requested mem props {1}", (uint32_t)MemTypeBitsMask, ReqMemPropFlags);
		exit(1);
		return -1;
	}

	bool HasStencilComponent(VkFormat Format)
	{
		return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT) ||
			(Format == VK_FORMAT_D24_UNORM_S8_UINT));
	}

	void ImageMemBarrier(const VkCommandBuffer& CmdBuf, VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount)
	{
		VkImageMemoryBarrier Barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = 0,
			.dstAccessMask = 0,
			.oldLayout = OldLayout,
			.newLayout = NewLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = Image,
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = (uint32_t)LayerCount
			}
		};

		VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
		VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

		if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
			(Format == VK_FORMAT_D16_UNORM) ||
			(Format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
			(Format == VK_FORMAT_D32_SFLOAT) ||
			(Format == VK_FORMAT_S8_UINT) ||
			(Format == VK_FORMAT_D16_UNORM_S8_UINT) ||
			(Format == VK_FORMAT_D24_UNORM_S8_UINT))
		{
			Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(Format)) {
				Barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_GENERAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
			NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} /* Convert back from read-only to updateable */
		else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} /* Convert from updateable texture to shader read-only */
		else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert depth texture from undefined state to depth-stencil buffer */
		else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} /* Wait for render pass to complete */
		else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = 0; // VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = 0;
			/*
					sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			///		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
					destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			*/
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert back from read-only to color attachment */
		else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		} /* Convert from updateable texture to shader read-only */
		else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert back from read-only to depth attachment */
		else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		} /* Convert from updateable depth texture to shader read-only */
		else if (OldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else {
			printf("Unknown Barrier case\n");
			exit(1);
		}

		vkCmdPipelineBarrier(CmdBuf, sourceStage, destinationStage,
			0, 0, NULL, 0, NULL, 1, &Barrier);
	}

	void BeginDynamicRendering(const VkCommandBuffer& CmdBuf, uint32_t ImageIndex, VkClearValue* pClearColor, VkClearValue* pDepthValue)
	{
		VkRenderingAttachmentInfoKHR ColorAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.pNext = NULL,
			.imageView = VK_Backend::Get()->GetSwapChain().GetImageView(ImageIndex),
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = pClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE
		};

		if (pClearColor)
		{
			ColorAttachment.clearValue = *pClearColor;
		}

		/*VkRenderingAttachmentInfo DepthAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = NULL,
			.imageView = VK_Backend::Get()->GetSwapChain().GetImageView(ImageIndex),
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = pDepthValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE
		};

		if (pDepthValue)
		{
			DepthAttachment.clearValue = *pDepthValue;
		}*/

		VkRenderingInfoKHR RenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.renderArea = {{0, 0}, {1440, 720}},
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &ColorAttachment,
			.pDepthAttachment = NULL
		};

		vkCmdBeginRendering(CmdBuf, &RenderingInfo);
	}

	VK_BufferAndMemory CreateBuffer(size_t pSize, VkBufferUsageFlags pUsage, VkMemoryPropertyFlags pMemProp)
	{
		VkBufferCreateInfo bCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = pSize,
			.usage = pUsage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};

		VK_BufferAndMemory bufferAndMem;

		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		// # Step 1: create a buffer
		VkResult res = vkCreateBuffer(pDevice, &bCreateInfo, NULL, &bufferAndMem.pBuffer);
		VK_CHECK("vkCreateBuffer", res);

		// # Step 2: get the buffer memory requitements
		VkMemoryRequirements MemReqs = {};
		vkGetBufferMemoryRequirements(pDevice, bufferAndMem.pBuffer, &MemReqs);

		bufferAndMem.pAllocationSize = MemReqs.size;

		// # Step 3: get the memory type index
		uint32_t MemoryTypeIndex = GetMemoryTypeIndex(MemReqs.memoryTypeBits, pMemProp);

		// # Step 4: allocate memory
		VkMemoryAllocateInfo MemAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = MemReqs.size,
			.memoryTypeIndex = MemoryTypeIndex
		};

		res = vkAllocateMemory(pDevice, &MemAllocInfo, NULL, &bufferAndMem.pMemory);
		VK_CHECK("vkAllocateMemory", res);

		// # Step 5: bind memory
		res = vkBindBufferMemory(pDevice, bufferAndMem.pBuffer, bufferAndMem.pMemory, 0);
		VK_CHECK("vkBindBufferMemory", res);

		return bufferAndMem;
	}

	void CopyBuffer(VkBuffer pDst, VkBuffer pSrc, VkDeviceSize pSize)
	{
		const VkCommandBuffer& pCopyCmdBuf = VK_Backend::Get()->GetCopyCmdBuf();

		BeginCommandBuffer(pCopyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy BufferCopy = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = pSize
		};

		vkCmdCopyBuffer(pCopyCmdBuf, pSrc, pDst, 1, &BufferCopy);

		vkEndCommandBuffer(pCopyCmdBuf);

		const VK_Queue& pQueue = VK_Backend::Get()->GetQueue();

		pQueue.SubmitSync(pCopyCmdBuf);

		pQueue.WaitIdle();
	}

}