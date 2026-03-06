#ifndef GPU_BACKEND_H
#define GPU_BACKEND_H

namespace GPU {

	class GPU_Backend
	{
	public:
		GPU_Backend() {}
		~GPU_Backend() {}

		virtual bool IsDeviceSupportBackend() = 0;

		virtual void Backend_Init() = 0;
		virtual void Backend_Exit() = 0;

		virtual void RenderBegin() = 0;
		virtual void RenderEnd() = 0;
	};

}

#endif