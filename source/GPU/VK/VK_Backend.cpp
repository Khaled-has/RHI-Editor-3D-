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
		pDevice.Create();
		pSwapChain.Create();
	}

	void VK_Backend::Backend_Exit()
	{
	}

	void VK_Backend::RenderBegin()
	{
	}

	void VK_Backend::RenderEnd()
	{
	}

}