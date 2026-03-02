#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include <stdio.h>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#define BIN(_N) (1 << _N)

namespace GPU {

	/*	 # Physical devises types	*/
	enum PhyDeviceType
	{
		NONE		= 0,
		INTEL_HD	= BIN(1),
		INTEL_UHD	= BIN(2),
		NIVDIA		= BIN(3),
		AMD			= BIN(4)
	};
	/*	 # Vulkan extensions	*/
	enum VKExtensions
	{
		VK_SURFACE_EXT_NAME		= BIN(5),
		VK_WIN32_SURFACE_EXT	= BIN(6),
		VK_LINUX_SURFACE_EXT	= BIN(7),
		VK_DEBUG_UTILS_EXT		= BIN(8)
	};

	//	 # Hard coded extensions used
	inline unsigned int VK_Extensions = VK_SURFACE_EXT_NAME | VK_WIN32_SURFACE_EXT | VK_DEBUG_UTILS_EXT;

	inline const std::vector<const char*> VK_Get_Extensions(unsigned int _ext)
	{
		std::vector<const char*> EXT;

		switch (_ext)
		{
		case VK_SURFACE_EXT_NAME:
			EXT.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			break;

		case VK_DEBUG_UTILS_EXT:
			EXT.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			break;

		case VK_WIN32_SURFACE_EXT:
			EXT.push_back("VK_KHR_win32_surface");
			break;

		case VK_LINUX_SURFACE_EXT:
			EXT.push_back("VK_KHR_xcb_surface");
		}

		return EXT;
	}
	
	/*	 # Vulkan layers	*/
	enum VKLayers
	{
		VK_VALIDATION_LAYER		= BIN(9)
	};

	//	 # Hard coded layers used
	inline unsigned int VK_Layers = VK_VALIDATION_LAYER;

	inline const std::vector<const char*> VK_Get_Layers(unsigned int _lyr)
	{
		std::vector<const char*> LYR;

		switch (_lyr)
		{
		case VK_VALIDATION_LAYER:
			LYR.push_back("VK_LAYER_KHRONOS_validation");
			break;
		}

		return LYR;
	}


	/*	 # Physical device	 */
	struct VK_PhysicalDevice
	{
		VkPhysicalDevice pPhysDevice;
		VkPhysicalDeviceProperties m_devProps;
		std::vector<VkQueueFamilyProperties> m_qFamilyProps;
		std::vector<VkBool32> m_qSupportsPresent;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		VkSurfaceCapabilitiesKHR m_surfaceCaps;
		VkPhysicalDeviceMemoryProperties m_memProps;
		std::vector<VkPresentModeKHR> m_presentModes;
		VkPhysicalDeviceFeatures m_features;
		VkFormat m_depthFormat;
		struct {
			int Variant = 0;
			int Major = 0;
			int Minor = 0;
			int Patch = 0;
		} m_apiVersion;
		std::vector<VkExtensionProperties> m_extensions;

		inline bool IsExtensionSupported(const char* pExt) const
		{
			return false;
		}
	};

	/*	 # The window types	  */
	enum WinType
	{
		NO = 0,
		SDL3 = 1,
		GLFW = 2
	};

	/*	 # The Device it self	*/
	class VK_Device
	{
	public:
		VK_Device() {}
		~VK_Device() {}

		// # Get the number of physical devices
		inline uint32_t GetPhyDevCount() const { return pPhyDevCount; }
		// # Get the name of running physical device
		inline std::string GetPhyDevName() const { return pPhyDevName; }
		// # Get the type of running physical device
		inline PhyDeviceType GetPhyDevType() const { return pPhyDevType; }

		// # Create vulkan context ( Instance - PhysicalDev - Device )
		void Create(WinType pWinType, void* pAppWin);

	private:
		VkInstance pInstance		= VK_NULL_HANDLE;
		VkSurfaceKHR pSurface		= VK_NULL_HANDLE;
		VkPhysicalDevice pPhyDevice = VK_NULL_HANDLE;
		VkDevice pDevice			= VK_NULL_HANDLE;

		/*	# Physical devices  */
		uint32_t pPhyDevCount	  = 0;
		std::string pPhyDevName   = "";
		PhyDeviceType pPhyDevType = NONE;

		std::vector<VK_PhysicalDevice> m_pPhyDevices;

		/* # Init functions */
		void CreateInstance();
		void CreateDebugCallMessenger();
		void CreateSurface(WinType pWinType, void* pAppWin);
		void CreatePhysicalDevice();
		void CreateDevice();
	};

}

#endif