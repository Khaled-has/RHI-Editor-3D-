#include "GPU_Batch.h"

#include "Log/Log.h"
#include "GPU_Backend.h"

#include "VK/VK_Batch.h"

namespace RHI
{
	GPU_Batch* CreateBatch(GPU_BatchTypes pBatchType, const GPU_Buffer* pVertexBuf, const GPU_Buffer* pUniformBuf, const GPU_Buffer* pIndexBuf)
	{
		if (GPU_Backend::GetBackendType() == GPU_BACKEND_TYPES::GPU_BACKEND_VULKAN)
		{
			return new GPU::VK_Batch(pBatchType, pVertexBuf, pUniformBuf, pIndexBuf);
		}
		else if (GPU_Backend::GetBackendType() == GPU_BACKEND_TYPES::GPU_BACKEND_DX12)
		{
			return NULL;
		}

		GPU_LOG_ERROR("Selected the backend api before you create any thing");
		return NULL;
	}
}