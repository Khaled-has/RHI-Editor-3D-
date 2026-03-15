#include "VK_Texture.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"
#include "VK_Buffer.h"

#include <stb_image.h>

namespace GPU
{

	VkSampler CreateTextureSampler(VkFilter MinFilter, VkFilter MaxFilter,
		VkSamplerAddressMode AddressMode)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		VkSamplerCreateInfo SamplerInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = MinFilter,
			.minFilter = MaxFilter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = AddressMode,
			.addressModeV = AddressMode,
			.addressModeW = AddressMode,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 1,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};

		VkSampler Sampler;
		VkResult res = vkCreateSampler(pDevice, &SamplerInfo, VK_NULL_HANDLE, &Sampler);
		VK_CHECK("vkCreateSampler", res);
		return Sampler;
	}

	VkImageView CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags, bool IsCubemap)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		VkImageViewCreateInfo ViewInfo =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = Image,
			.viewType = IsCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D,
			.format = Format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange = {
				.aspectMask = AspectFlags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = IsCubemap ? 6u : 1u
			}
		};

		VkImageView ImageView;
		VkResult res = vkCreateImageView(pDevice, &ViewInfo, NULL, &ImageView);
		if (res != VK_SUCCESS)
		{
			printf("Cannot create image view\n");
		}

		return ImageView;
	}

	void CopyBufferToImage(VkImage Dst, VkBuffer Src, uint32_t ImageWidth, uint32_t ImageHeight, VkDeviceSize LayerSize, int LayerCount)
	{
		const VkCommandBuffer& CmdBuf = VK_Backend::Get()->GetCopyCmdBuf();

		BeginCommandBuffer(CmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		std::vector<VkBufferImageCopy> BufferImageCopy(LayerCount);

		for (int i = 0; i < LayerCount; i++)
		{
			VkBufferImageCopy bic = {
				.bufferOffset = i * LayerSize,
				.bufferRowLength = 0,
				.bufferImageHeight = 0,
				.imageSubresource = VkImageSubresourceLayers{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = 0,
					.baseArrayLayer = (uint32_t)i,
					.layerCount = (uint32_t)LayerCount
				},
				.imageOffset = VkOffset3D{.x = 0, .y = 0, .z = 0},
				.imageExtent = VkExtent3D{.width = ImageWidth, .height = ImageHeight, .depth = 1}
			};

			BufferImageCopy[i] = bic;
		}

		vkCmdCopyBufferToImage(
			CmdBuf, Src, Dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			(uint32_t)BufferImageCopy.size(), BufferImageCopy.data());

		vkEndCommandBuffer(CmdBuf);

		VK_Backend::Get()->GetQueue().SubmitSync(CmdBuf);

		VK_Backend::Get()->GetQueue().WaitIdle();
	}

	int GetBytesPerTexFormat(VkFormat Format)
	{
		switch (Format)
		{
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_UNORM:
			return 1;
		case VK_FORMAT_R16_SFLOAT:
			return 2;
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_SRGB:
			return 4;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return 4 * sizeof(uint16_t);
		case VK_FORMAT_R32G32B32_SFLOAT:
			return 3 * sizeof(float);
		case VK_FORMAT_R8G8B8_SRGB:
			return 3;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return 4 * sizeof(float);
		default:
			VK_LOG_ERROR("Unknown format: {0}", (uint32_t)Format);
			exit(1);
		}
		return 0;
	}

	void VK_Texture::Create(const char* pFilename)
	{
		int w, h, c;

		// # Step 1: load the image pixels
		stbi_uc* pPixels = stbi_load((std::string(RES_PATH) + pFilename).c_str(), &w, &h, &c, STBI_rgb_alpha);
		ImageWidth = w; ImageHeight = h; ImageChannels = c;
		if (!pPixels)
		{
			VK_LOG_ERROR("Error loading texture from {0}", pFilename);
		}

		// # Step 2: create the image object and populate it with pixels
		VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM; // Hard coded for now.
		CreateTextureImageFromData(pPixels, Format, false); // Hard coded for now.

		// # Step 3: release the image pixels. we don't need them after this point
		stbi_image_free(pPixels);

		// # Step 4: create image view
		VkImageAspectFlags AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT; // Hard coded for now
		pView = CreateImageView(pImage, Format, AspectFlags, false);

		VkFilter MinFilter = VK_FILTER_LINEAR;
		VkFilter MaxFilter = VK_FILTER_LINEAR;
		VkSamplerAddressMode AddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		// # Step 5: create the texture sampler
		pSampler = CreateTextureSampler(MinFilter, MaxFilter, AddressMode);
	}

	void VK_Texture::Destroy()
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		vkFreeMemory(pDevice, pMemory, NULL);
		vkDestroyImageView(pDevice, pView, NULL);
		vkDestroySampler(pDevice, pSampler, NULL);
		vkDestroyImage(pDevice, pImage, NULL);
	}

	void VK_Texture::CreateTextureImageFromData(const void* pPixels, VkFormat pFormat, bool IsCubemap)
	{
		VkImageUsageFlagBits Usage = (VkImageUsageFlagBits)(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		VkMemoryPropertyFlagBits PropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		CreateImage(pFormat, Usage, PropertyFlags, IsCubemap);

		UpdateTextureImage(pFormat, 1, pPixels, IsCubemap);
	}

	void VK_Texture::CreateImage(VkFormat pFormat, VkImageUsageFlags UsageFlags, VkMemoryPropertyFlagBits PropertyFlags, bool IsCubemap)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		VkImageCreateInfo ImageInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = NULL,
			.flags = IsCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : (VkImageCreateFlags)0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = pFormat,
			.extent = VkExtent3D{.width = ImageWidth, .height = ImageHeight, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = IsCubemap ? 6u : 1u,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = UsageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = NULL,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		// # Step 1: create the image object
		VkResult res = vkCreateImage(pDevice, &ImageInfo, NULL, &pImage);
		VK_CHECK("vkCreateImage", res);

		// # Step 2: get the buffer memory requirements
		VkMemoryRequirements MemReqs = { 0 };
		vkGetImageMemoryRequirements(pDevice, pImage, &MemReqs);

		// # Step 3: get the memory type index
		uint32_t MemoryTypeIndex = GetMemoryTypeIndex(MemReqs.memoryTypeBits, PropertyFlags);

		// # Step 4: allocate memory
		VkMemoryAllocateInfo MemAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = NULL,
			.allocationSize = MemReqs.size,
			.memoryTypeIndex = MemoryTypeIndex
		};

		res = vkAllocateMemory(pDevice, &MemAllocInfo, NULL, &pMemory);
		VK_CHECK("vkAllocateMemory", res);

		// # Step 5: bind memory
		res = vkBindImageMemory(pDevice, pImage, pMemory, 0);
		VK_CHECK("vkBindImageMemory", res);
	}

	void VK_Texture::UpdateTextureImage(VkFormat pFormat, int LayerCount, const void* pPixels, bool IsCubemap)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		int BytesPerPixel = GetBytesPerTexFormat(pFormat);

		VkDeviceSize LayerSize = ImageWidth * ImageHeight * BytesPerPixel;
		VkDeviceSize ImageSize = LayerCount * LayerSize;

		VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags Properties =
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VK_BufferAndMemory StagingTex = CreateBuffer(ImageSize, Usage, Properties);

		void* pMem = NULL;
		VkResult res = vkMapMemory(pDevice, StagingTex.pMemory, 0, ImageSize, 0, &pMem);
		VK_CHECK("vkMapMemory", res);
		memcpy(pMem, pPixels, ImageSize);
		vkUnmapMemory(pDevice, StagingTex.pMemory);

		TransitionImageLayout(pFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, LayerCount);

		CopyBufferToImage(pImage, StagingTex.pBuffer, ImageWidth, ImageHeight, LayerSize, LayerCount);

		TransitionImageLayout(pFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, LayerCount);

		vkFreeMemory(pDevice, StagingTex.pMemory, NULL);
		vkDestroyBuffer(pDevice, StagingTex.pBuffer, NULL);
	}

	void VK_Texture::TransitionImageLayout(VkFormat pFormat, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount)
	{
		const VkCommandBuffer& CmdBuf = VK_Backend::Get()->GetCopyCmdBuf();

		BeginCommandBuffer(CmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		ImageMemBarrier(CmdBuf, pImage, pFormat, OldLayout, NewLayout, LayerCount);

		vkEndCommandBuffer(CmdBuf);

		VK_Backend::Get()->GetQueue().SubmitSync(CmdBuf);

		VK_Backend::Get()->GetQueue().WaitIdle();
	}

}