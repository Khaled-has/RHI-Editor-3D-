#include "VK_Draw.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

namespace GPU
{
	void VK_Draw::SetBuffer(const RHI::GPU_Buffer* pBuffer, RHI::GPU_BufferTypes pBufType, uint32_t pBindIndex)
	{
		if (pBufType == RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE)
		{
			pBindings.push_back(
				VK_PipelineBinding{
					.pBinding = pBindIndex,
					.pDescType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pStageFlag = VK_SHADER_STAGE_VERTEX_BIT,
					.pBindingType = VK_BINDING_BUFFER_INFO,
					.pBuffer = *(VK_Buffer*)pBuffer
				}
			);
		}
		else if (pBufType == RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM)
		{
			std::vector<VK_Buffer> pUniforms(VK_Backend::Get()->GetSwapChain().GetImageCount(), *(VK_Buffer*)pBuffer);

			pBindings.push_back(
				VK_PipelineBinding{
					.pBinding = pBindIndex,
					.pDescType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.pStageFlag = VK_SHADER_STAGE_VERTEX_BIT,
					.pBindingType = VK_BINDING_UNIFORM_INFO,
					.pUniformBuffers = pUniforms
				}
			);
		}
	}

	void VK_Draw::SetTexture(const RHI::GPU_Texture* pTexture, uint32_t pBindIndex)
	{
		pBindings.push_back(
			VK_PipelineBinding{
				.pBinding = pBindIndex,
				.pDescType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pStageFlag = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pBindingType = VK_BINDING_IMAGE_INFO,
				.pTexture = *(const VK_Texture*)pTexture
			}
		);
	}

	void VK_Draw::Create()
	{
		pShader.Create("test.vert", "test.frag");

		pGraphPipeline.Create(&pBindings, pShader.GetVertexShader(), pShader.GetFragmentShader());
		RecordCommandBuffer(BIND_COMMAND_DRAW_FN(&VK_Draw::DrawCommand));
	}

	void VK_Draw::Destroy()
	{
		pShader.Destroy();
		pGraphPipeline.Destroy();
	}

	void VK_Draw::Draw(uint32_t pFirstVertex, uint32_t pVertexCount)
	{
		// # Rerecord if any thing change
		if (this->pFirstVertex != pFirstVertex || this->pVertexCount != pVertexCount)
		{
			this->pFirstVertex = pFirstVertex;
			this->pVertexCount = pVertexCount;

			RecordCommandBuffer(BIND_COMMAND_DRAW_FN(&VK_Draw::DrawCommand));
		}
	}

	void VK_Draw::DrawCommand(const VkCommandBuffer& CmdBuf, uint32_t ImageIndex)
	{
		pGraphPipeline.Bind(ImageIndex);

		vkCmdDraw(CmdBuf, pVertexCount, 1, pFirstVertex, 0);
	}
}