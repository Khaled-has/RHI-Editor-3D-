#include "TestRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>

#include "Log/Log.h"

struct
{
	glm::mat4 model;
	glm::mat4 VP;
} pUniform;

TestRenderer::TestRenderer()
{
	pBackend = RHI::CreateVulkanBackend();
	pBackend->Backend_Init();

	pDraw = RHI::CreateDraw();

	// Vertices buffer
	const std::vector<float> pVertices = {
		//   Position		   UVs
		-5.0f, -5.0f, 0.0f, 0.0, 0.0,
		5.0f, -5.0f, 0.0f,  1.0, 0.0,
		5.0f, 5.0f, 0.0f,  1.0, 1.0,

		-5.0f, -5.0f, 0.0f, 0.0, 0.0,
		5.0f, 5.0f, 0.0f,  1.0, 1.0,
		-5.0, 5.0, 0.0,     0.0, 1.0,

		//   The second quad
		-5.0f, -5.0f, -5.0f, 0.0, 0.0,
		5.0f, -5.0f, -5.0f,  1.0, 0.0,
		5.0f, 5.0f, -5.0f,  1.0, 1.0,
		-5.0, 5.0, -5.0f,     0.0, 1.0
	};
	pStorageBuffer = RHI::CreateBuffer(pVertices.data(), sizeof(float) * pVertices.size(), RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE);
	pDraw->SetBuffer(pStorageBuffer, RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE, 0);

	// Indices
	std::vector<unsigned int> pIndices = {
		0, 1, 2,
		2, 0, 3,
	};
	//pIndexBuffer = RHI::CreateBuffer(pIndices.data(), size_t(sizeof(unsigned int) * pIndices.size()), RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE);
	//pDraw->SetBuffer(pIndexBuffer, RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE, 1);

	// Uniform buffer
	pUniform.model = glm::mat4(1);
	pUniform.VP = pCamera.getProj() * pCamera.getView(1.0);

	pUniBuffer = RHI::CreateBuffer(&pUniform, size_t(sizeof(pUniform)), RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM);
	pDraw->SetBuffer(pUniBuffer, RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM, 1);
	
	// Texture
	pTexture = RHI::CreateTexture("Test.jpg");
	pDraw->SetTexture(pTexture, 2);

	// Draw command
	pDraw->Create();

	// Create render pass 1
	RHI::GPU_RenderPassInfo RenderInfo;

	pRenderPass_1 = RHI::CreateRenderPass(RenderInfo);

	// Record draw command
	pBackend->BeginRecord();

	pRenderPass_1->Begin();

	pDraw->Draw(0, 6);

	pRenderPass_1->End();

	pBackend->EndRecord(pRenderPass_1);
}

TestRenderer::~TestRenderer()
{
	pStorageBuffer->Destroy();
	pUniBuffer->Destroy();
	pTexture->Destroy();
	pDraw->Destroy();

	pBackend->Backend_Exit();
}

void TestRenderer::Update()
{
	static float ro = 0.0f;
	ro += 1.0f;
	pUniform.model = glm::rotate(glm::mat4(1), glm::radians(ro), glm::vec3(0, 0, 1));

	// DeltaTime
	static Uint64 lastTime = SDL_GetPerformanceCounter();
	static Uint64 currentTime;
	static double deltaTime;

	currentTime = SDL_GetPerformanceCounter();
	deltaTime = (double)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
	lastTime = currentTime;
	pUniform.VP = pCamera.getProj() * pCamera.getView(deltaTime);
	

	pUniBuffer->Update(&pUniform, sizeof(pUniform));

	pBackend->Rendering();
}
