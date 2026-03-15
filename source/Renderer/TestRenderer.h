#ifndef TESTRENDERER_H
#define TESTRENDERER_H

#include "GPU/GPU.h"

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
	RHI::GPU_Buffer* pUniBuffer;
};

#endif