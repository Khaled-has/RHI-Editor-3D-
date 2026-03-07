#include "VK_CommandBufferPool.h"

#include "VK_Backend.h"
#include "VK_wrappar.h"

namespace GPU
{

	void VK_CommandBufferPool::Create()
	{
		const VK_Device& pDevice = VK_Backend::Get()->GetDevice();

		VkCommandPoolCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = pDevice.GetSelectedQFamily()
		};

		VkResult res = vkCreateCommandPool(pDevice.GetDevice(), &CreateInfo, NULL, &pCommandPool);
		VK_CHECK("vkCreateCommandPool", res);
	}

	void VK_CommandBufferPool::Destroy()
	{
		vkDestroyCommandPool(VK_Backend::Get()->GetDevice().GetDevice(), pCommandPool, NULL);
	}

	void VK_CommandBufferPool::CreateCommandBuffers(uint32_t Count, VkCommandBuffer* CmdBufs)
	{
		VkCommandBufferAllocateInfo AllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = NULL,
			.commandPool = pCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = Count
		};

		VkResult res = vkAllocateCommandBuffers(
			VK_Backend::Get()->GetDevice().GetDevice(), &AllocateInfo, CmdBufs
		);
		VK_CHECK("vkAllocateCommandBuffers", res);
	}

	void VK_CommandBufferPool::FreeCommandBuffers(uint32_t Count, VkCommandBuffer* CmdBufs)
	{
		vkFreeCommandBuffers(
			VK_Backend::Get()->GetDevice().GetDevice(), pCommandPool,
			Count, CmdBufs
		);
	}

	void BeginCommandBuffer(const VkCommandBuffer& CmdBuf, VkCommandBufferUsageFlags UsageFlags)
	{
		VkCommandBufferBeginInfo Info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = UsageFlags,
			.pInheritanceInfo = NULL
		};

		VkResult res = vkBeginCommandBuffer(CmdBuf, &Info);
		VK_CHECK("vkBeginCommandBuffer", res);
	}

}