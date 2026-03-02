#ifndef VK_WRAPPIR_H
#define VK_WRAPPIR_H

#include "Log/Log.h"

#define VK_CHECK(_des, _res) if (_res != VK_SUCCESS) GPU_LOG_ERROR("GPU ( VK Backend ) Error: {0}  \n\t   at line ( {1} )  \n\t   file ( {2} )", _des, __LINE__, __FILE__);

#define VK_LOG_INFO(...) GPU_LOG_INFO(""); GPU_LOG_INFO("GPU : VK_Backend # "); GPU_LOG_INFO(__VA_ARGS__);
#define VK_LOG_ERROR(...) GPU_LOG_ERROR(""); GPU_LOG_ERROR("GPU : VK_Backend # Error: "); GPU_LOG_ERROR(__VA_ARGS__);

#endif