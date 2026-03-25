#ifndef TESTRENDERER_H
#define TESTRENDERER_H

#include "GPU/GPU.h"

#include "GPU/shaderCompiler/SC_Compiler.h"

#include "Camera.h"

class TestRenderer
{
public:
	TestRenderer();
	~TestRenderer();

	void Update();

private:
	RHI::GPU_Backend* pBackend;

	RHI::GPU_Draw* pDraw;
	RHI::GPU_Buffer* pStorageBuffer;
	RHI::GPU_Buffer* pIndexBuffer;
	RHI::GPU_Buffer* pUniBuffer;
	RHI::GPU_Texture* pTexture;
	RHI::GPU_RenderPass* pRenderPass_1;

	Camera pCamera;

	SC::SC_Compiler compile;
};

#endif