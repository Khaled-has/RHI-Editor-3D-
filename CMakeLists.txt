cmake_minimum_required(VERSION 3.26)

project("RHI-Editor-3D-")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/extern/glfw/lib/cmake/glfw3")
find_package(glfw3 CONFIG REQUIRED)

set(GLFW_PATH "${CMAKE_SOURCE_DIR}/extern/glfw")

# Vulkan
set(VULKAN_PATH "${CMAKE_SOURCE_DIR}/extern/Vulkan")

if(MSVC) 
	add_compile_options("/utf-8")
endif()

# Executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
 "source/EnteryPoint.cpp"
 "source/GPU/VK/VK_Backend.cpp" "source/Log/Log.cpp" "source/GPU/VK/VK_Device.cpp")

# Target include directories
target_include_directories(${PROJECT_NAME} PRIVATE
						"${CMAKE_SOURCE_DIR}/source"				    # source
						"${GLFW_PATH}/include"						    # glfw
						"${VULKAN_PATH}/include"					    # VK
						"${CMAKE_SOURCE_DIR}/extern/spdlog/include"     # Spdlog
)

# Target link libararise
target_link_libraries(${PROJECT_NAME} PRIVATE
						"${VULKAN_PATH}/Lib/vulkan-1.lib"
						"${GLFW_PATH}/lib/glfw3.lib"
)

# Definons
add_compile_definitions(GPU_DEBUG TRUE)