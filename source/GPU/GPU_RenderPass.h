#ifndef GPU_RENDERPASS_H
#define GPU_RENDERPASS_H

#include <iostream>

#define BIN_(x) 1 << x

namespace RHI
{

	enum class GPU_ColorAttachments
	{
		NONE = 0,
		GPU_COLOR_ATTACHMENT_RGBA16 = BIN_(1),
		GPU_COLOR_ATTACHMENT_RGBA32 = BIN_(2)
	};

	enum class GPU_DepthAttachments
	{
		NONE = 0,
		GPU_DEPTH_ATTACHMENT_FLOAT_16 = BIN_(1),
		GPU_DEPTH_ATTACHMENT_FLOAT_32 = BIN_(2),
	};

	struct GPU_RenderPassInfo
	{
		bool pEnableColor = false;
		bool pEnableDepth = false;
		uint32_t pColorAttachmentCount = 0;
		GPU_ColorAttachments pColorAttachments;
		GPU_DepthAttachments pDepthAttachment;
	};

	class GPU_RenderPass
	{
	public:
		GPU_RenderPass() {}
		~GPU_RenderPass() {}

		virtual void Destroy() = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;

	private:
		virtual void Create(const GPU_RenderPassInfo pInfo) = 0;
	};

	GPU_RenderPass* CreateRenderPass(GPU_RenderPassInfo pInfo);

}

#endif