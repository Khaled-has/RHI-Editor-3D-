cmake_minimum_required(VERSION 3.26)

project("RHI-Editor-3D-")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDL3
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/extern/SDL3")
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)

# Vulkan
set(VULKAN_PATH "${CMAKE_SOURCE_DIR}/extern/Vulkan")

if(MSVC) 
	add_compile_options("/utf-8")
endif()

# Executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
 "source/EnteryPoint.cpp"
 "source/GPU/VK/VK_Backend.cpp" "source/Log/Log.cpp" "source/GPU/VK/VK_Device.cpp" "source/Window/windows/WindowsWindow.cpp" "source/GPU/VK/VK_SwapChain.cpp" "source/GPU/VK/VK_CommandBufferPool.h" "source/GPU/VK/VK_CommandBufferPool.cpp" "source/GPU/VK/VK_Thread.cpp" 
 "source/GPU/VK/VK_Queue.cpp"
 "source/GPU/VK/VK_Batch.cpp" "source/GPU/VK/VK_GraphicsPipeline.cpp" "source/GPU/VK/VK_Buffer.cpp" "source/GPU/VK/VK_wrappar.cpp" "source/GPU/VK/VK_Shader.cpp"
 "extern/stb/stb_image.cpp" "source/GPU/GPU_Buffer.cpp" "source/GPU/GPU_Batch.cpp" "source/GPU/VK/VK_Texture.cpp" "source/GPU/GPU_Texture.h" "source/GPU/GPU_Texture.cpp" "source/GPU/VK/VK_Draw.h" "source/GPU/GPU_Draw.h" "source/GPU/VK/VK_Draw.cpp" "source/GPU/GPU_Draw.cpp" "source/Renderer/TestRenderer.h" "source/GPU/GPU.h" "source/Renderer/TestRenderer.cpp" "source/GPU/GPU_Backend.cpp"
 "source/Renderer/Camera.cpp"  "source/GPU/shaderCompiler/SC_Compiler.h" "source/GPU/shaderCompiler/SC_Compiler.cpp" "source/GPU/VK/VK_RenderPass.h" "source/GPU/GPU_RenderPass.h" "source/GPU/VK/VK_RenderPass.cpp" "source/GPU/GPU_RenderPass.cpp")

# Target include directories
target_include_directories(${PROJECT_NAME} PRIVATE
						"${CMAKE_SOURCE_DIR}/source"				    # source
						"${VULKAN_PATH}/include"					    # VK
						"${CMAKE_SOURCE_DIR}/extern/shaderc"			# shaderc
						"${CMAKE_SOURCE_DIR}/extern/spdlog/include"     # Spdlog
						"${CMAKE_SOURCE_DIR}/extern/glm"				# glm
						"${CMAKE_SOURCE_DIR}/extern/stb"				# stb
)

# Target link libararise
target_link_libraries(${PROJECT_NAME} PRIVATE
						"${VULKAN_PATH}/Lib/vulkan-1.lib"
						SDL3::SDL3
						"${CMAKE_SOURCE_DIR}/extern/shaderc/shaderc_shared.lib"
)

# If Windows we will use SDL3
if(WIN32)

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_if_different
						"${CMAKE_SOURCE_DIR}/extern/SDL3/lib/x64/SDL3.dll"
							$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)

	add_compile_definitions(SDL_WIN)

endif()

# Definons
add_compile_definitions(GPU_DEBUG=TRUE)
add_compile_definitions(RES_PATH="${CMAKE_SOURCE_DIR}/res/")