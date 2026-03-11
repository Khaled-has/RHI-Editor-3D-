#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

namespace RHI
{
	enum class GPU_BufferTypes
	{
		GPU_BUFFER_STORAGE,
		GPU_BUFFER_UNIFORM
	};

	class GPU_Buffer
	{
	public:
		GPU_Buffer() {}
		~GPU_Buffer() {}

		virtual void Create(const void* pData, size_t pSize, GPU_BufferTypes pBufferType) = 0;
		virtual void Destroy() = 0;

		virtual void Update(const void* pData, size_t pSize) = 0;

	private:

	};

	GPU_Buffer* CreateBuffer(const void* pData, size_t pSize, GPU_BufferTypes pBufType);

}

#endif