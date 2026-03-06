#ifndef VK_COMMANDBUFFERPOOL_H
#define VK_COMMANDBUFFERPOOL_H

#include <vulkan/vulkan.h>

namespace GPU
{

	class VK_CommandBufferPool
	{
	public:
		VK_CommandBufferPool() {}
		~VK_CommandBufferPool() {}

		void Create();
		void CreateCommandBuffers(uint32_t Count, VkCommandBuffer* CmdBufs);

	private:
		VkCommandPool pCommandPool = VK_NULL_HANDLE;
	};

	void BeginCommandBuffer(const VkCommandBuffer& CmdBuf, VkCommandBufferUsageFlags UsageFlags);

}

#endif