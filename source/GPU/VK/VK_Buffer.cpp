#include "VK_Buffer.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{

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

	void VK_Buffer::Create(const void* pData, size_t pSize, RHI::GPU_BufferTypes pBufferType)
	{
		// # Storage buffer
		if (pBufferType == RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE)
		{
			CreateStorage(pData, pSize);
		}
		// # Uniform buffer
		else if (pBufferType == RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM)
		{
			CreateUniform(pData, pSize);
		}
	}

	void VK_Buffer::CreateStorage(const void* pData, size_t pSize)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		// # Step 1: create the staging buffer
		VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags MemProps =
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VK_BufferAndMemory StaginBuffer = CreateBuffer(pSize, Usage, MemProps);

		// # Step 2: map the memory if the stage buffer
		void* pMem = NULL;
		VkDeviceSize Offset = 0;
		VkMemoryMapFlags Flags = 0;
		VkResult res = vkMapMemory(
			pDevice, StaginBuffer.pMemory, Offset,
			StaginBuffer.pAllocationSize, Flags, &pMem
		);
		VK_CHECK("vkMapMemory", res);

		// # Step 3: copy the data to the stating buffer
		memcpy(pMem, pData, pSize);

		// # Step 4: unMap/release the mapped memory
		vkUnmapMemory(pDevice, StaginBuffer.pMemory);

		// # Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		pBufferAndMemory = CreateBuffer(pSize, Usage, MemProps);

		// # Step 6: copy the staging buffer to the final buffer
		CopyBuffer(pBufferAndMemory.pBuffer, StaginBuffer.pBuffer, pSize);

		// # Step 7: release the resources of the staging buffer
		vkFreeMemory(pDevice, StaginBuffer.pMemory, NULL);
		vkDestroyBuffer(pDevice, StaginBuffer.pBuffer, NULL);
	}

	void VK_Buffer::CreateUniform(const void* pData, size_t pSize)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		// # Step 1: create the staging buffer
		VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags MemProps =
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		pBufferAndMemory = CreateBuffer(pSize, Usage, MemProps);

		// # Step 2: map the memory
		void* pMem = NULL;
		VkResult res = vkMapMemory(pDevice, pBufferAndMemory.pMemory, 0, pSize, 0, &pMem);
		VK_CHECK("vkMapMemory", res);

		memcpy(pMem, pData, pSize);
		vkUnmapMemory(pDevice, pBufferAndMemory.pMemory);
	}

	void VK_Buffer::Destroy()
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		vkFreeMemory(pDevice, pBufferAndMemory.pMemory, NULL);
		vkDestroyBuffer(pDevice, pBufferAndMemory.pBuffer, NULL);
	}

	void VK_Buffer::Update(const void* pData, size_t pSize)
	{
		const VkDevice& pDevice = VK_Backend::Get()->GetDevice().GetDevice();

		void* pMem = NULL;
		VkResult res = vkMapMemory(pDevice, pBufferAndMemory.pMemory, 0, pSize, 0, &pMem);
		VK_CHECK("vkMapMemory", res);

		memcpy(pMem, pData, pSize);
		vkUnmapMemory(pDevice, pBufferAndMemory.pMemory);
	}

}
