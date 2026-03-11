#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

#include "GPU/GPU_Buffer.h"

namespace GPU
{

	struct VK_BufferAndMemory {
		VkBuffer pBuffer = VK_NULL_HANDLE;
		VkDeviceMemory pMemory = VK_NULL_HANDLE;
		VkDeviceSize pAllocationSize = 0;
	};

	class VK_Buffer : public RHI::GPU_Buffer
	{
	public:
		VK_Buffer() {}
		VK_Buffer(const void* pData, size_t pSize, RHI::GPU_BufferTypes pBufferType)
		{
			Create(pData, pSize, pBufferType);
		}
		~VK_Buffer() {}

		virtual void Create(const void* pData, size_t pSize, RHI::GPU_BufferTypes pBufferType) override;
		void Destroy();

		virtual void Update(const void* pData, size_t pSize) override;

		inline const VK_BufferAndMemory& GetBuffer() const { return pBufferAndMemory; }

	private:
		VK_BufferAndMemory pBufferAndMemory;

		void CreateStorage(const void* pData, size_t pSize);
		void CreateUniform(const void* pData, size_t pSize);
	};

}

#endif