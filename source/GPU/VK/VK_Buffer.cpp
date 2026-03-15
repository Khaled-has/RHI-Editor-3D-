#include "VK_Buffer.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{

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
