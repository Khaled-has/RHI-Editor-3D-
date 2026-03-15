#ifndef GPU_BATCH_H
#define GPU_BATCH_H

#include <iostream>

#include "GPU_Buffer.h"
#include "GPU_Texture.h"

namespace RHI
{

	enum class GPU_BatchTypes
	{
		GPU_BATCH_TRIANGLES,
		GPU_BATCH_QUADS,
	};

	class GPU_Batch
	{
	public:
		GPU_Batch() {}
		~GPU_Batch() {}

		virtual void Create(
			GPU_BatchTypes pBatchType, const GPU_Buffer* pVertexBuf, const GPU_Buffer* pUniformBuf, 
			const GPU_Texture* pTexture, const GPU_Buffer* pIndexBuf = NULL
		) = 0;
		virtual void Destroy() = 0;
		
		virtual void Draw() = 0;
	};

	GPU_Batch* CreateBatch(
		GPU_BatchTypes pBatchType, const GPU_Buffer* pVertexBuf, const GPU_Buffer* pUniformBuf, 
		const GPU_Texture* pTexture, const GPU_Buffer* pIndexBuf = NULL
	);

}

#endif