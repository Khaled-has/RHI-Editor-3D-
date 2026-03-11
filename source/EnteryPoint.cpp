#include <stdio.h>

#include "Log/Log.h"

#ifdef WIN32
#include "Window/windows/WindowsWindow.h"
#endif

#include "GPU/VK/VK_Backend.h"
#include "GPU/VK/VK_Buffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(int argc, char* argv[])
{
	Log::Init();

	Win::Window* pAppWin = nullptr;

#ifdef WIN32
	pAppWin = Win::CreateWindowsWindow(
		"RHI Editor ( 3D )",
		1440, 720
	);
#endif
	pAppWin->Create();

	RHI::GPU_Backend* gpu_backend = RHI::CreateVulkanBackend();
	gpu_backend->Backend_Init();

	const std::vector<float> vert = { -0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0, 0.5, 0.0 };
	RHI::GPU_Buffer* buffer = RHI::CreateBuffer(
		vert.data(), size_t(sizeof(vert) * vert.size()), RHI::GPU_BufferTypes::GPU_BUFFER_STORAGE
	);

	glm::mat4 model = glm::mat4(1);
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 0, 1));
	RHI::GPU_Buffer* uniBuffer = RHI::CreateBuffer(&model, sizeof(glm::mat4), RHI::GPU_BufferTypes::GPU_BUFFER_UNIFORM);

	RHI::GPU_Batch* batch = RHI::CreateBatch(RHI::GPU_BatchTypes::GPU_BATCH_TRIANGLES, buffer, uniBuffer);

	// For test
	SDL_Event ev;
	bool pRun = true;
	while (pRun)
	{
		bool cl = false;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				pRun = false;
			else if (ev.key.key == SDLK_ESCAPE)
				pRun = false;
			else if (ev.key.key == SDLK_K)
				cl = true;
		}

		gpu_backend->RenderBegin();

		/*if (cl)
		{
			glm::mat4 model = glm::mat4(1);
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 0, 1));
			uniBuffer->Update(&model, sizeof(glm::mat4));
		}*/
		
		batch->Draw();

		gpu_backend->RenderEnd();
	}

	uniBuffer->Destroy();
	buffer->Destroy();
	batch->Destroy();

	gpu_backend->Backend_Exit();

	return 0;
}