#include "VK_Backend.h"

namespace GPU {

	VK_Backend* VK_Backend::pVkInstance = nullptr;

	VK_Backend::VK_Backend()
	{
		pVkInstance = this;
	}

	bool VK_Backend::IsDeviceSupportBackend()
	{
		return false;
	}

	void VK_Backend::Backend_Init()
	{
		// 1 # Create device
		pDevice.Create();
		// 2 # Create swapchain
		pSwapChain.Create();
		// 3 # Create command buffer pool
		pCmdBufPool.Create();
		// 4 # Create command buffers
		pCmdBufs.resize(pSwapChain.GetImageCount());
		pCmdBufPool.CreateCommandBuffers(pSwapChain.GetImageCount(), pCmdBufs.data());
		pCmdBufPool.CreateCommandBuffers(1, &pCopyCmdBuf);
		// 5 # Create load thread
		pLoadThread.Create();
		// 6 # Create queue
		pQueue.Create();
	}

	void VK_Backend::Backend_Exit()
	{
		// # Destroy queue
		pQueue.Destroy();
		// # Destroy swapchain
		pSwapChain.Destroy();
		// # Free command buffers
		pCmdBufPool.FreeCommandBuffers((uint32_t)pCmdBufs.size(), pCmdBufs.data());
		// # Destroy command buffer pool
		pCmdBufPool.Destroy();
		// # Destroy load thread
		pLoadThread.Destroy();
		// # Destroy device
		pDevice.Destroy();
	}

	void VK_Backend::RenderBegin()
	{
	}

	void VK_Backend::RenderEnd()
	{
	}

}