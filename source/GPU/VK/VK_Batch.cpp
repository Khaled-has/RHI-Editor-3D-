#include "VK_Batch.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

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

		RecordCommandBuffers();
	}

	void VK_Batch::Destroy()
	{
		pShader.Destroy();
		pGraphPipeline.Destroy();
	}

	void VK_Batch::Draw()
	{
		VK_Queue& pQueue = VK_Backend::Get()->GetQueue();

		uint32_t ImageIndex = pQueue.AcquireNextImage();

		pQueue.SubmitAsync(&(VK_Backend::Get()->GetCmdBuf(ImageIndex)));

		pQueue.Present(ImageIndex);
	}

	void VK_Batch::RecordCommandBuffers()
	{
		const VkSurfaceFormatKHR pFormat = VK_Backend::Get()->GetSwapChain().GetSurfaceFormat();

		for (uint32_t i = 0; i < VK_Backend::Get()->GetSwapChain().GetImageCount(); i++)
		{
			const VkCommandBuffer& CmdBuf = VK_Backend::Get()->GetCmdBuf(i);
			const VkImage& Image = VK_Backend::Get()->GetSwapChain().GetImage(i);

			BeginCommandBuffer(CmdBuf, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			ImageMemBarrier(
				CmdBuf, Image, pFormat.format,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1
			);

			// # Begin rendering with dynamic only rendering for now
			VkClearValue ClearColor = {
				.color = {1.0, 0.0, 0.0, 1.0}
			};
			VkClearValue DepthValue = {
				.depthStencil = {.depth = 1.0f, .stencil = 0}
			};

			BeginDynamicRendering(CmdBuf, i, &ClearColor, &DepthValue);

			pGraphPipeline.Bind(i);

			vkCmdDraw(CmdBuf, 3, 1, 0, 0);

			vkCmdEndRendering(CmdBuf);

			ImageMemBarrier(
				CmdBuf, Image, pFormat.format,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1
			);

			VkResult res = vkEndCommandBuffer(CmdBuf);
			VK_CHECK("vkEndCommandBuffer", res);
		}
	}
}