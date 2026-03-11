#include "VK_Batch.h"

#include "VK_wrappar.h"
#include "VK_Backend.h"

#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>

namespace GPU
{
	struct Uni
	{
		glm::mat4 model = glm::mat4(1);
		glm::mat4 MVP = glm::mat4(1);
	};
	Uni matrix;

	void VK_Batch::Create(
		RHI::GPU_BatchTypes pBatchType, const RHI::GPU_Buffer* pVertexBuf, const RHI::GPU_Buffer* pUniformBuf, const RHI::GPU_Buffer* pIndexBuf
	)
	{
		// # Triangles batch
		if (pBatchType == RHI::GPU_BatchTypes::GPU_BATCH_TRIANGLES)
		{
			CreateTrianglesPipeline(
				(VK_Buffer*)pVertexBuf, (VK_Buffer*)pUniformBuf, 
				pIndexBuf == NULL ? NULL : (VK_Buffer*)pIndexBuf
			);
		}

		// # Record command buffers
		RecordCommandBuffer(BIND_COMMAND_DRAW_FN(&GPU::VK_Batch::DrawCommand));
	}

	void VK_Batch::Destroy()
	{
		pGraphPipeline.Destroy();
		pShader.Destroy();

		for (uint32_t i = 0; i < pUniforms.size(); i++) 
			pUniforms[i].Destroy();
	}

	void VK_Batch::Draw()
	{
		static Uint64 lastTime = SDL_GetPerformanceCounter();
		static Uint64 currentTime;
		static double deltaTime;

		// DeltaTime
		currentTime = SDL_GetPerformanceCounter();
		deltaTime = (double)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
		lastTime = currentTime;

		VK_Queue& pQueue = VK_Backend::Get()->GetQueue();

		uint32_t ImageIndex = pQueue.AcquireNextImage();

		//static float rot = 0.0f;
		//rot += 90.0f * deltaTime;
		//matrix.model = glm::mat4(1);
		//matrix.model = glm::rotate(matrix.model, glm::radians(rot), glm::vec3(0, 0, 1));
		//for (uint32_t i = 0; i < 3; i++)
		//{
		//	pUniforms[i].Update(&matrix, sizeof(Uni));
		//}

		pQueue.SubmitAsync(&(VK_Backend::Get()->GetCmdBuf(ImageIndex)));

		pQueue.Present(ImageIndex);
	}

	void VK_Batch::DrawCommand(const VkCommandBuffer& CmdBuf, uint32_t ImageIndex)
	{
		pGraphPipeline.Bind(ImageIndex);
		vkCmdDraw(CmdBuf, 3, 1, 0, 0);
	}

	void VK_Batch::CreateTrianglesPipeline(const VK_Buffer* pVertexBuf, const VK_Buffer* pUniformBuf, const RHI::GPU_Buffer* pIndexBuf)
	{
		pShader.Create("test.vert", "test.frag");
	
		std::vector<VK_PipelineBinding> infos;
		std::vector<VK_Buffer> uniforms(VK_Backend::Get()->GetSwapChain().GetImageCount(), *pUniformBuf);
		infos.push_back(
			VK_PipelineBinding{
				.pBinding = 0,
				.pDescType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pStageFlag = VK_SHADER_STAGE_VERTEX_BIT,
				.pBindingType = VK_BINDING_BUFFER_INFO,
				.pBuffer = pVertexBuf
			});
		infos.push_back(
			VK_PipelineBinding{
				.pBinding = 1,
				.pDescType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pStageFlag = VK_SHADER_STAGE_VERTEX_BIT,
				.pBindingType = VK_BINDING_UNIFORM_INFO,
				.pUniformBuffers = &uniforms
			}
		);

		pGraphPipeline.Create(&infos, pShader.GetVertexShader(), pShader.GetFragmentShader());
	}
}