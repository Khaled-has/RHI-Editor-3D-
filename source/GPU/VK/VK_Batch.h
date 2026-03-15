#ifndef VK_BATCH_H
#define VK_BATCH_H

#include <iostream>
#include <vector>

#include "VK_GraphicsPipeline.h"
#include "VK_Buffer.h"
#include "VK_Texture.h"
#include "VK_Shader.h"

#include "GPU/GPU_Batch.h"

namespace GPU
{

	class VK_Batch : public RHI::GPU_Batch
	{
	public:
		VK_Batch() {}
		VK_Batch(
			RHI::GPU_BatchTypes pBatchType, 
			const RHI::GPU_Buffer* pVertexBuf, const RHI::GPU_Buffer* pUniformBuf, 
			const RHI::GPU_Texture* pTexture, const RHI::GPU_Buffer* pIndexBuf = NULL
		)
		{
			Create(pBatchType, pVertexBuf, pUniformBuf, pTexture, pIndexBuf);
		}
		~VK_Batch() {}

		virtual void Create(
			RHI::GPU_BatchTypes pBatchType, 
			const RHI::GPU_Buffer* pVertexBuf, const RHI::GPU_Buffer* pUniformBuf, 
			const RHI::GPU_Texture* pTexture, const RHI::GPU_Buffer* pIndexBuf = NULL
		) override;
		virtual void Destroy() override;

		virtual void Draw() override;

	private:
		VK_GraphicsPipeline pGraphPipeline;

		std::vector<VK_Buffer> pUniforms;
		VK_Shader pShader;

		void DrawCommand(const VkCommandBuffer& CmdBuf, uint32_t ImageIndex);

		void CreateTrianglesPipeline(const VK_Buffer* pVertexBuf, const VK_Buffer* pUniformBuf, const VK_Texture* pTexture, const RHI::GPU_Buffer* pIndexBuf);
	};

}

#endif