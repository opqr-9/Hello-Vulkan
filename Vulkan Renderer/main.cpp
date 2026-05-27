#include "VulkanBase.h"

#include <stdio.h>
#include <stdlib.h>

void InitVulkan()
{
	/*
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createCommandBuffers();
	createSyncObjects();
	*/
	GraphicsBase::base.UseLatestApiVersion();	
	GraphicsBase::base.CreateVkInstance();
	GraphicsBase::base.CreateDeBugMessenger();
	GraphicsBase::base.CreateSurface(pWindow);
	GraphicsBase::base.PickPhysicalDevice();
	GraphicsBase::base.CreateDevice();
	GraphicsBase::base.InitSwapchain();
	GraphicsBase::base.CreateRenderPass();
	GraphicsBase::base.CreateDescriptorSetLayout();
	GraphicsBase::base.CreateGraphicsPipeline();
	GraphicsBase::base.CreateCommandPool();
	GraphicsBase::base.CreateDepthResources();
	GraphicsBase::base.CreateFramebuffers();
	GraphicsBase::base.CreateTextureImages();
	GraphicsBase::base.CreateTextureImageView();
	GraphicsBase::base.CreateTextureSampler();
	GraphicsBase::base.LoadModel();
	InitGameObjects();
	GraphicsBase::base.CreateVertexBuffer();
	GraphicsBase::base.CreateIndexBuffer();
	GraphicsBase::base.CreateUniformBuffers();
	GraphicsBase::base.CreateDescriptorPool();
	GraphicsBase::base.CreateDescriptorSets();
	GraphicsBase::base.CreateCommandBuffers();
	GraphicsBase::base.CreateSyncObjects();
	GraphicsBase::base.initUIResources();
}

void MainLoop()
{
	float deltaTime = 0.02;
	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		// Update input system
		InputSystem::Update(deltaTime);

		if (GraphicsBase::base.ImGuiUtil.NewFrame())
		{
			//GraphicsBase::base.ImGuiUtil.handleKey();
			GraphicsBase::base.ImGuiUtil.updateBuffers();
		}


		GraphicsBase::base.DrawFrame();


		TitleFPS();
	}
}

int main()
{
	InitWindow();

	InitVulkan();

	InputSystem::Initialize();

	MainLoop();

	GraphicsBase::base.CleanUp();
}