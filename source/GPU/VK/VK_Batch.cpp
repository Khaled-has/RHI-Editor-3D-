#include "VK_Batch.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

#include <functional>

namespace GPU
{
	void VK_Batch::Create()
	{
		pShader.Create("test.vert", "test.frag");

		const std::vector<float> pVertices = {
			-0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.0, -0.5, 0.0
		};

		pBuffer.Create(pVertices.data(), size_t(sizeof(float) * pVertices.size()));

		std::vector<VK_PipelineBinding> infos;
		infos.push_back(
			VK_PipelineBinding{
				.pBinding = 0,
				.pDescType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pStageFlag = VK_SHADER_STAGE_VERTEX_BIT,
				.pBindingType = VK_BINDING_BUFFER_INFO,
				.pBuffer = &pBuffer
			});

		pGraphPipeline.Create(&infos, pShader.GetVertexShader(), pShader.GetFragmentShader());

		// # Record command buffers
		RecordCommandBuffer(BIND_COMMAND_DRAW_FN(&GPU::VK_Batch::DrawCommand));
	}

	void VK_Batch::Destroy()
	{
		pGraphPipeline.Destroy();
		pShader.Destroy();
		pBuffer.Destroy();
	}

	void VK_Batch::Draw()
	{
		VK_Queue& pQueue = VK_Backend::Get()->GetQueue();

		uint32_t ImageIndex = pQueue.AcquireNextImage();

		pQueue.SubmitAsync(&(VK_Backend::Get()->GetCmdBuf(ImageIndex)));

		pQueue.Present(ImageIndex);
	}

	void VK_Batch::DrawCommand(const VkCommandBuffer& CmdBuf, uint32_t ImageIndex)
	{
		pGraphPipeline.Bind(ImageIndex);
		vkCmdDraw(CmdBuf, 3, 1, 0, 0);
	}
}