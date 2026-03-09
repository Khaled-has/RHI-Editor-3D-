#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#include "GPU/GPU_Backend.h"

#include "VK_Device.h"
#include "VK_SwapChain.h"
#include "VK_CommandBufferPool.h"
#include "VK_Queue.h"
#include "VK_Thread.h"
#include "VK_Batch.h"

namespace GPU {

	class VK_Backend : public GPU_Backend
	{
	public:
		VK_Backend();
		~VK_Backend() {}

		virtual bool IsDeviceSupportBackend() override;

		virtual void Backend_Init() override;
		virtual void Backend_Exit() override;

		virtual void RenderBegin() override;
		virtual void RenderEnd() override;

		/*	# The instance of Vulkan backend  */
		static VK_Backend* Get() { return pVkInstance; }

		/*	# The backend cores	 */
		inline const VK_Device& GetDevice() { return pDevice; }
		inline const VK_SwapChain& GetSwapChain() { return pSwapChain; }
		inline const VK_CommandBufferPool& GetCmdBufPool() { return pCmdBufPool; }
		inline VK_Queue& GetQueue() { return pQueue; }
		inline const VK_Thread& GetLoadThread() { return pLoadThread; }

		inline const VkCommandBuffer& GetCmdBuf(uint32_t Index) { return pCmdBufs[Index]; }
		inline const VkCommandBuffer& GetCopyCmdBuf() const { return pCopyCmdBuf; }
	private:
		static VK_Backend* pVkInstance;

		VK_Device pDevice;
		VK_SwapChain pSwapChain;
		VK_CommandBufferPool pCmdBufPool;
		VK_Queue pQueue;
		VK_Thread pLoadThread;

		std::vector<VkCommandBuffer> pCmdBufs;
		VkCommandBuffer pCopyCmdBuf = VK_NULL_HANDLE;	// It's hard coded, I will use another thread later on.

		VK_Batch pBatch;
	};

	inline VK_Backend* CreateVulkanBackend() { return new VK_Backend; }

}

#endif