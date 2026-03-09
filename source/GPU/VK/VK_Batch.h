#ifndef VK_BATCH_H
#define VK_BATCH_H

#include <iostream>
#include <vector>

#include "VK_GraphicsPipeline.h"
#include "VK_Buffer.h"
#include "VK_Shader.h"

namespace GPU
{

	class VK_Batch
	{
	public:
		VK_Batch() {}
		~VK_Batch() {}

		void Create();
		void Destroy();

		void Draw();

	private:
		VK_GraphicsPipeline pGraphPipeline;
		VK_Buffer pBuffer;
		VK_Shader pShader;

		void RecordCommandBuffers();
	};

}

#endif