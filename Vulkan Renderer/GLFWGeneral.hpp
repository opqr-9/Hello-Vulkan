#pragma once
#define GLFW_INCLUDE_VULKAN	//启用GLFW对Vulkan的原生支持
#include <GLFW/glfw3.h>		//GLFW库的主头文件
#include "VulkanBase.h"

GLFWwindow* pWindow;
GLFWmonitor* pMonitor;
const char* title = "VulkanRenderer";

void InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	pWindow = glfwCreateWindow(windowSize.width, windowSize.height, title, nullptr, nullptr);
}

void TitleFPS() {
	static double curTime = glfwGetTime();
	static double preTime = curTime;
	static int dframe = 0;
	static std::stringstream info;
	curTime = glfwGetTime();
	dframe++;
	if (curTime - preTime >= 1)
	{
		info << title << " " << dframe / (curTime - preTime) << " FPS";
		glfwSetWindowTitle(pWindow, info.str().c_str());
		info.str("");
		preTime = curTime;
		dframe=0;
	}
}