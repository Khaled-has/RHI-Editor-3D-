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
 "source/GPU/VK/VK_Backend.cpp" "source/Log/Log.cpp" "source/GPU/VK/VK_Device.cpp" "source/Window/windows/WindowsWindow.cpp" "source/GPU/VK/VK_SwapChain.cpp" "source/GPU/VK/VK_CommandBufferPool.h" "source/GPU/VK/VK_CommandBufferPool.cpp")

# Target include directories
target_include_directories(${PROJECT_NAME} PRIVATE
						"${CMAKE_SOURCE_DIR}/source"				    # source
						"${VULKAN_PATH}/include"					    # VK
						"${CMAKE_SOURCE_DIR}/extern/spdlog/include"     # Spdlog
)

# Target link libararise
target_link_libraries(${PROJECT_NAME} PRIVATE
						"${VULKAN_PATH}/Lib/vulkan-1.lib"
						SDL3::SDL3
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
add_compile_definitions(GPU_DEBUG TRUE)