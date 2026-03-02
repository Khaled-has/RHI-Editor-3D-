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

	/*	 # The Device it self	*/
	class VK_Device
	{
	public:
		VK_Device() {}
		~VK_Device() {}

		// # Get the name of running physical device
		inline std::string GetPhyDevName() const { return pPhyDevName; }
		// # Get the type of running physical device
		inline PhyDeviceType GetPhyDevType() const { return pPhyDevType; }

		// # Create vulkan context ( Instance - PhysicalDev - Device )
		void Create();

	private:
		VkInstance pInstance		= VK_NULL_HANDLE;
		VkPhysicalDevice pPhyDevice = VK_NULL_HANDLE;
		VkDevice pDevice			= VK_NULL_HANDLE;

		std::string pPhyDevName   = "";
		PhyDeviceType pPhyDevType = NONE;


		/* # Init functions */
		void CreateInstance();
		void CreatePhysicalDevice();
		void CreateDevice();
	};

}

#endif