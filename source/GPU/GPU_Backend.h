#ifndef GPU_BACKEND_H
#define GPU_BACKEND_H

namespace GPU {

	class GPU_Backend
	{
	public:
		GPU_Backend() {}
		~GPU_Backend() {}

		virtual void Backend_Init() = 0;
		virtual void Backend_Exit() = 0;

		virtual void Render_Begin() = 0;
		virtual void Render_End() = 0;
	};

}

#endif