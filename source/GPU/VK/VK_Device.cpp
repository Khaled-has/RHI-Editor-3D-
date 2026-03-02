#include "VK_Device.h"

#include "VK_wrappir.h"

//#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace GPU
{

	void VK_Device::Create(WinType pWinType, void* pAppWin)
	{
		CreateInstance();
		CreateSurface(pWinType, pAppWin);
		CreatePhysicalDevice();
	}

	void VK_Device::CreateInstance()
	{

		VkApplicationInfo AppInfo;
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext = NULL;
		AppInfo.pApplicationName = "RHI Editor ( 3D )";
		AppInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		AppInfo.pEngineName = "RHI Editor ( 3D )";
		AppInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		AppInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
		

		std::vector<const char*> Layers		= VK_Get_Layers(VK_Layers);
		std::vector<const char*> Extensions = VK_Get_Extensions(VK_Extensions);

		VkInstanceCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0;
		CreateInfo.pApplicationInfo = &AppInfo;
		CreateInfo.enabledLayerCount = (uint32_t)Layers.size();
		CreateInfo.ppEnabledLayerNames = Layers.data();
		CreateInfo.enabledExtensionCount = (uint32_t)Extensions.size();
		CreateInfo.ppEnabledExtensionNames = Extensions.data();

		VkResult res = vkCreateInstance(&CreateInfo, NULL, &pInstance);
		VK_CHECK("vkCreateInstance", res);
	}
	
	void VK_Device::CreateSurface(WinType pWinType, void* pAppWin)
	{
		// SDL3 window
		if (pWinType == WinType::SDL3)
		{
			if (SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(pAppWin), pInstance, NULL, &pSurface))
			{
				VK_LOG_ERROR("Cannot create SDL3 surface");
				exit(1);
			}
		}
		// glfw window
		else if (pWinType == WinType::GLFW)
		{
			/*if (glfwCreateWindowSurface(pInstance, static_cast<GLFWwindow*>(pAppWin), NULL, &pSurface))
			{
				VK_LOG_ERROR("Cannot create glfw surface");
				exit(1);
			}*/
		}

#ifdef WIN32
		VK_LOG_INFO("Windows surface created");
#elif  __linux__
		VK_LOG_INFO("linux surface created");
#endif
	}

	void VK_Device::CreatePhysicalDevice()
	{

		/*	 # Get the physical devices on this device	 */
		VkResult res = vkEnumeratePhysicalDevices(pInstance, &pPhyDevCount, NULL);
		VK_CHECK("vkEnumeratePhysicalDevices", res);

		VK_LOG_INFO("Physical devices count: {0}", pPhyDevCount);

		m_pPhyDevices.resize(pPhyDevCount);

		std::vector<VkPhysicalDevice> Devices(pPhyDevCount);

		res = vkEnumeratePhysicalDevices(pInstance, &pPhyDevCount, Devices.data());
		VK_CHECK("vkEnumeratePhysicalDevices", res);
	}

}
