#include "VK_Device.h"

#include "VK_wrappir.h"

namespace GPU
{

	void VK_Device::Create()
	{
		CreateInstance();
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

}
