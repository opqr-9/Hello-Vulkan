#pragma once
//可能会用上的C++标准库
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>

//GLM
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//如果你惯用左手坐标系，在此定义GLM_FORCE_LEFT_HANDED
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/hash.hpp>


#include <chrono>

//stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Vulkan
#ifdef _WIN32                        //考虑平台是Windows的情况（请自行解决其他平台上的差异）
#define VK_USE_PLATFORM_WIN32_KHR    //在包含vulkan.h前定义该宏，会一并包含vulkan_win32.h和windows.h
#define NOMINMAX                     //定义该宏可避免windows.h中的min和max两个宏与标准库中的函数名冲突
#pragma comment(lib, "vulkan-1.lib") //链接编译所需的静态存根库
#endif
#include <vulkan/vulkan.h>
//#include <vulkan/vulkan_raii.hpp>
