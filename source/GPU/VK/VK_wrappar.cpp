#include "VK_wrappar.h"

#include "VK_Backend.h"

namespace GPU
{
	uint32_t GetMemoryTypeIndex(uint32_t MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
	{
		const VkPhysicalDeviceMemoryProperties& MemProps = VK_Backend::Get()->GetDevice().GetSelectedDevice().m_memProps;

		for (uint32_t i = 0; i < MemProps.memoryTypeCount; i++)
		{
			const VkMemoryType& MemType = MemProps.memoryTypes[i];
			uint32_t CurBitmask = (1 << i);
			bool IsCurMemTypeSupported = (MemTypeBitsMask & CurBitmask);
			bool HasRequiredMemProps = ((MemType.propertyFlags & ReqMemPropFlags) == ReqMemPropFlags);

			if (IsCurMemTypeSupported && HasRequiredMemProps)
			{
				return i;
			}
		}

		VK_LOG_ERROR("Cannot find memory type for type {0} requested mem props {1}", (uint32_t)MemTypeBitsMask, ReqMemPropFlags);
		exit(1);
		return -1;
	}
}