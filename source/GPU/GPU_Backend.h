#ifndef GPU_BACKEND_H
#define GPU_BACKEND_H

#include "GPU_Texture.h"
#include "GPU_RenderPass.h"

namespace RHI {

	enum class GPU_BACKEND_TYPES
	{
		GPU_BACKEND_VULKAN,
		GPU_BACKEND_DX12
	};

	class GPU_Backend
	{
	public:
		GPU_Backend() {}
		~GPU_Backend() {}

		virtual bool IsDeviceSupportBackend() = 0;

		virtual void Backend_Init() = 0;
		virtual void Backend_Exit() = 0;

		virtual void BeginRecord() = 0;
		virtual void EndRecord(RHI::GPU_RenderPass* pFinalRenderPass) = 0;

		virtual void Rendering() = 0;

		inline static GPU_BACKEND_TYPES& GetBackendType() { return pBackendType; }

	private:
		inline static GPU_BACKEND_TYPES pBackendType;
	};

	GPU_Backend* CreateVulkanBackend();

}

#endif