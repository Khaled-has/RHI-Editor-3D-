#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

namespace GPU
{

	struct VK_BufferAndMemory {
		VkBuffer pBuffer = VK_NULL_HANDLE;
		VkDeviceMemory pMemory = VK_NULL_HANDLE;
		VkDeviceSize pAllocationSize = 0;
	};

	class VK_Buffer
	{
	public:
		VK_Buffer() {}
		~VK_Buffer() {}

		void Create(const void* pData, size_t pSize);
		void Destroy();

		void Update(const void* pData, size_t pSize);

		const VK_BufferAndMemory& GetBuffer() { return pBufferAndMemory; }

	private:
		VK_BufferAndMemory pBufferAndMemory;
	};

}

#endif