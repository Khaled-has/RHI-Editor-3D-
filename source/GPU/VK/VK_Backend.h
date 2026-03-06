#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#include "GPU/GPU_Backend.h"

#include "VK_Device.h"
#include "VK_SwapChain.h"
#include "VK_CommandBufferPool.h"

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

	private:
		static VK_Backend* pVkInstance;

		VK_Device pDevice;
		VK_SwapChain pSwapChain;
		VK_CommandBufferPool pCmdBufPool;
	};

	inline VK_Backend* CreateVulkanBackend() { return new VK_Backend; }

}

#endif