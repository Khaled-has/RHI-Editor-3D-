#include "VK_Device.h"

#include "VK_wrappir.h"

//#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace GPU
{

	static const char* PrintImageUsageFlags(const VkImageUsageFlags& flags)
	{

		if (flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		{
			return "Image usage transfer src is supported";
		}

		if (flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		{
			return "Image usage transfer dest is supported";
		}

		if (flags & VK_IMAGE_USAGE_SAMPLED_BIT)
		{
			return "Image usage sampled is supported";
		}

		if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			return "Image usage color attachment is supported";
		}

		if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return "Image usage depth stencil attachment is supported";
		}

		if (flags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
		{
			return "Image usage transient attachment is supported";
		}

		if (flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
			return "Image usage input attachment is supported";
		}

		return "Image usage unknown";
	}

	static const char* PrintMemoryProperty(VkMemoryPropertyFlags PropertyFlags)
	{
		if (PropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			return "DEVICE LOCAL ";
		}

		if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
			return "HOST VISIBLE ";
		}

		if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
			return "HOST COHERENT ";
		}

		if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
			return "HOST CACHED ";
		}

		if (PropertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
			return "LAZILY ALLOCATED ";
		}

		if (PropertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) {
			return "PROTECTED ";
		}

		return "unknown";
	}

	static VkFormat FindSupportedFormat(VkPhysicalDevice Device, const std::vector<VkFormat>& Candidates,
		VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for (int i = 0; i < Candidates.size(); i++)
		{
			VkFormat Format = Candidates[i];
			VkFormatProperties Props;
			vkGetPhysicalDeviceFormatProperties(Device, Format, &Props);

			if ((Tiling == VK_IMAGE_TILING_LINEAR) &&
				(Props.linearTilingFeatures & Features) == Features)
			{
				return Format;
			}
			else if ((Tiling == VK_IMAGE_TILING_OPTIMAL) &&
				(Props.optimalTilingFeatures & Features) == Features)
			{
				return Format;
			}
		}

		printf("Failed to find supported format!\n");
		exit(1);
	}

	static VkFormat FindDepthFormat(VkPhysicalDevice Device)
	{
		std::vector<VkFormat> Candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};

		VkFormat DepthFormat = FindSupportedFormat(
			Device, Candidates, VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);

		VK_LOG_INFO("Device depth format: {0} ",
			DepthFormat == VK_FORMAT_D32_SFLOAT ? "VK_FORMAT_D32_SFLOAT" : "",
			DepthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ? "VK_FORMAT_D32_SFLOAT_S8_UINT" : "",
			DepthFormat == VK_FORMAT_D24_UNORM_S8_UINT ? "VK_FORMAT_D24_UNORM_S8_UINT" : ""
		);

		return DepthFormat;
	}

	void VK_Device::Create(WinType pWinType, void* pAppWin)
	{
		CreateInstance();
		CreateSurface(pWinType, pAppWin);
		CreatePhysicalDevice();
	}

	const VK_PhysicalDevice& VK_Device::GetPhysicalDevices(uint32_t Index)
	{
		return m_pPhyDevices[Index];
	}

	const VK_PhysicalDevice& VK_Device::GetSelectedDevice()
	{
		return m_pPhyDevices[pSelectedPhyDevIndex];
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
		AppInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);


		std::vector<const char*> Layers = VK_Get_Layers(VK_Layers);
		std::vector<const char*> Extensions = VK_Get_Extensions(VK_Extensions);
		for (int i = 0; i < Extensions.size(); i++) { VK_LOG_INFO("{0}", Extensions[i]); }
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

	void VK_Device::CreateDebugCallMessenger()
	{
	}

	void VK_Device::CreateSurface(WinType pWinType, void* pAppWin)
	{
		// SDL3 window
		if (pWinType == WinType::SDL3)
		{
			if (!SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(pAppWin), pInstance, NULL, &pSurface))
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

		for (uint32_t i = 0; i < pPhyDevCount; i++)
		{
			VkPhysicalDevice PhyDev = Devices[i];

			m_pPhyDevices[i].pPhysDevice = PhyDev;

			// # Device properties
			vkGetPhysicalDeviceProperties(PhyDev, &m_pPhyDevices[i].m_devProps);

			VK_LOG_INFO("Device name: {0} | Device type: {1}", m_pPhyDevices[i].m_devProps.deviceName, GetDeviceType(m_pPhyDevices[i].m_devProps.deviceType));

			// # Device API version
			uint32_t apiV = m_pPhyDevices[i].m_devProps.apiVersion;
			m_pPhyDevices[i].m_apiVersion.Major = VK_API_VERSION_VARIANT(apiV);
			m_pPhyDevices[i].m_apiVersion.Minor = VK_API_VERSION_VARIANT(apiV);
			m_pPhyDevices[i].m_apiVersion.Patch = VK_API_VERSION_VARIANT(apiV);

			VK_LOG_INFO("Api Version Supported: {0}.{1}.{2}", m_pPhyDevices[i].m_apiVersion.Minor, m_pPhyDevices[i].m_apiVersion.Minor, m_pPhyDevices[i].m_apiVersion.Patch);

			// # Device extensions
			uint32_t ExtensionsCount;
			vkEnumerateDeviceExtensionProperties(m_pPhyDevices[i].pPhysDevice, NULL, &ExtensionsCount, NULL);

			m_pPhyDevices[i].m_extensions.resize(ExtensionsCount);

			vkEnumerateDeviceExtensionProperties(m_pPhyDevices[i].pPhysDevice, NULL, &ExtensionsCount, m_pPhyDevices[i].m_extensions.data());

			VK_LOG_INFO("Physical device extensions");
			/*for (const VkExtensionProperties& e : m_pPhyDevices[i].m_extensions)
			{
				VK_LOG_INFO("	{0}", e.extensionName);
			}*/

			// # Device Queue families properties
			uint32_t NumQFamilies = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &NumQFamilies, NULL);
			VK_LOG_INFO("	Num of queue family: {0}", NumQFamilies);

			m_pPhyDevices[i].m_qFamilyProps.resize(NumQFamilies);
			m_pPhyDevices[i].m_qSupportsPresent.resize(NumQFamilies);

			vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &NumQFamilies, m_pPhyDevices[i].m_qFamilyProps.data());

			for (uint32_t q = 0; q < NumQFamilies; q++)
			{
				const VkQueueFamilyProperties& QFamilyProp = m_pPhyDevices[i].m_qFamilyProps[q];

				VK_LOG_INFO("	Family {0} Num queues: {1} ", q, QFamilyProp.queueCount);
				VkQueueFlags Flags = QFamilyProp.queueFlags;
				VK_LOG_INFO("	GFX {0}, Compute {1}, Transfer {2}, Sparse binding {3}",
					(Flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
					(Flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
					(Flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
					(Flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No"
				);

				res = vkGetPhysicalDeviceSurfaceSupportKHR(PhyDev, q, pSurface, &(m_pPhyDevices[i].m_qSupportsPresent[q]));
				VK_CHECK("vkGetPhysicalDeviceSurfaceSupportKHR", res);

				VK_LOG_INFO("	 This Queue Support Present: {0}", m_pPhyDevices[i].m_qSupportsPresent[q] ? "Yes" : "No");
			}

			// # Device surface formats
			uint32_t NumFormats = 0;
			res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDev, pSurface, &NumFormats, NULL);
			VK_CHECK("vkGetPhysicalDeviceSurfaceFormatsKHR ( 1 )", res);
			assert(NumFormats > 0);

			m_pPhyDevices[i].m_surfaceFormats.resize(NumFormats);

			res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDev, pSurface, &NumFormats, m_pPhyDevices[i].m_surfaceFormats.data());
			VK_CHECK("vkGetPhysicalDeviceSurfaceFormatsKHR ( 2 )", res);

			/*for (int j = 0; j < NumFormats; j++)
			{
				const VkSurfaceFormatKHR& SurfaceFormat = m_pPhyDevices[i].m_surfaceFormats[j];
				VK_LOG_INFO("	 Format {0} color space {1}", SurfaceFormat.format, SurfaceFormat.colorSpace);
			}*/

			// # Device surface capabilities
			res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhyDev, pSurface, &(m_pPhyDevices[i].m_surfaceCaps));
			VK_CHECK("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", res);

			VK_LOG_INFO(PrintImageUsageFlags(m_pPhyDevices[i].m_surfaceCaps.supportedUsageFlags));

			// # Device present modes
			uint32_t NumPresentModes = 0;

			res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhyDev, pSurface, &NumPresentModes, NULL);
			VK_CHECK("vkGetPhysicalDeviceSurfacePresentModesKHR ( 1 )", res);
			assert(NumPresentModes != 0);

			m_pPhyDevices[i].m_presentModes.resize(NumPresentModes);

			res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhyDev, pSurface, &NumPresentModes, m_pPhyDevices[i].m_presentModes.data());
			VK_CHECK("vkGetPhysicalDeviceSurfacePresentModesKHR ( 2 )", res);

			VK_LOG_INFO("Number of presentation modes {0}", NumPresentModes);

			// # Device memory properties
			vkGetPhysicalDeviceMemoryProperties(PhyDev, &(m_pPhyDevices[i].m_memProps));

			VK_LOG_INFO("Num memory types {0}", m_pPhyDevices[i].m_memProps.memoryTypeCount);
			for (uint32_t j = 0; j < m_pPhyDevices[i].m_memProps.memoryTypeCount; j++)
			{
				VK_LOG_INFO("{0}: flags {1} heap {2} ", j,
					PrintMemoryProperty(m_pPhyDevices[i].m_memProps.memoryTypes[j].propertyFlags),
					m_pPhyDevices[i].m_memProps.memoryTypes[j].heapIndex
					);
			}

			// # Device features
			vkGetPhysicalDeviceFeatures(PhyDev, &m_pPhyDevices[i].m_features);

			// # Device depth format
			m_pPhyDevices[i].m_depthFormat = FindDepthFormat(PhyDev);
		}
	}
	void VK_Device::CreateDevice()
	{
	}
}