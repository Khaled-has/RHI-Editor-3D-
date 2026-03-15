#include "TestRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
		//   Position		   Color
		-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0,
		0.5f, -0.5f, 0.0f,  0.0, 1.0, 0.0,
		0.0f, 0.5f, 0.0f,  0.0, 0.0, 1.0
	};
	pStorageBuffer = RHI::CreateBuffer(pVertices.data(), sizeof(float) * pVertices.size(), RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE);
	pDraw->SetBuffer(pStorageBuffer, RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE, 0);

	// Uniform buffer
	pUniform.model = glm::mat4(1);
	pUniform.VP = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f);

	pUniBuffer = RHI::CreateBuffer(&pUniform, size_t(sizeof(pUniform)), RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM);
	pDraw->SetBuffer(pUniBuffer, RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM, 1);

	// Draw command
	pDraw->Create();

}

TestRenderer::~TestRenderer()
{
	pStorageBuffer->Destroy();
	pUniBuffer->Destroy();
	pDraw->Destroy();

	pBackend->Backend_Exit();
}

void TestRenderer::Update()
{
	pBackend->RenderBegin();

	static float ro = 0.0f;
	ro += 1.0f;
	pUniform.model = glm::rotate(glm::mat4(1), glm::radians(ro), glm::vec3(0, 0, 1));

	pUniBuffer->Update(&pUniform, sizeof(pUniform));

	pDraw->Draw(0, 3);

	pBackend->RenderEnd();
}
