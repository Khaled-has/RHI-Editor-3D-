#include <stdio.h>

#include "Log/Log.h"

#include "GPU/VK/VK_Device.h"

int main(int argc, char* argv[])
{
	Log::Init();

	GPU::VK_Device test;
	test.Create();

	GPU_LOG_INFO("Hello RHI Editor");

	return 0;
}