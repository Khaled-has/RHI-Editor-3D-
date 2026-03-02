#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#include "GPU/GPU_Backend.h"

namespace GPU {

	class VK_Backend : public GPU_Backend
	{
	public:
		VK_Backend() {}
		~VK_Backend() {}

		virtual void Backend_Init() override;
		virtual void Backend_Exit() override;

		virtual void Render_Begin() override;
		virtual void Render_End() override;
	private:

	};

}

#endif