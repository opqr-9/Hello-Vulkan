#pragma once
#include "GLFWGeneral.hpp"

ImVec2 curPos;

VkVertexInputBindingDescription ImGuiVertexInputBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.stride = sizeof(ImDrawVert);
	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> ImGuiVertexInputAttributeDescription()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescription(3);
	attributeDescription[0].binding = 0;
	attributeDescription[0].location = 0;
	attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescription[0].offset = offsetof(ImDrawVert, pos);

	attributeDescription[1].binding = 0;
	attributeDescription[1].location = 1;
	attributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescription[1].offset = offsetof(ImDrawVert, uv);

	attributeDescription[2].binding = 0;
	attributeDescription[2].location = 2;
	attributeDescription[2].format = VK_FORMAT_R8G8B8A8_UNORM;
	attributeDescription[2].offset = offsetof(ImDrawVert, col);

	return attributeDescription;
}

class ImGuiVulkanUtil {
public:
	VkSampler sampler;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;
	VkImage fontImage;
	VkDeviceMemory fontImageMemory;
	VkImageView fontImageView;

	VkRenderPass renderPass;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;

	VkDevice* device;
	VkPhysicalDevice* physicalDevice;
	VkQueue* graphicsQueue;
	uint32_t graphicsQueueFamily = 0;

	ImGuiStyle vulkanStyle;

	struct PushConstant
	{
		glm::vec2 scale;
		glm::vec2 translate;
	};

	PushConstant pushConstant;

	VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(*physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
	}
public:
	ImGuiVulkanUtil(){}
	ImGuiVulkanUtil(VkDevice& device, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue, uint32_t graphicsQueueFamily)
		:device(&device), physicalDevice(&physicalDevice), graphicsQueue(&graphicsQueue), graphicsQueueFamily(graphicsQueueFamily){};
	~ImGuiVulkanUtil(){}


	void setStyle(uint32_t index)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		switch (index) {
		case 0:
			style = vulkanStyle;
			break;
		case 1:
			ImGui::StyleColorsClassic();
			break;
		case 2:
			ImGui::StyleColorsDark();
			break;
		case 3:
			ImGui::StyleColorsLight();
			break;
		}
	}

	void ImGuiInit(float width, float height)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();


		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		io.DisplaySize = ImVec2(width, height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		io.IniFilename = nullptr;

		vulkanStyle = ImGui::GetStyle();
		vulkanStyle.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
		vulkanStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		vulkanStyle.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		vulkanStyle.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		vulkanStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

		setStyle(0);

		pushConstant.translate = glm::vec2(-1.0f);
	}

	void InitDescriptorSets()
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &poolSize;

		vkCreateDescriptorPool(*device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.bindingCount = 1;
		descriptorSetLayoutCreateInfo.pBindings = &binding;

		vkCreateDescriptorSetLayout(*device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

		vkAllocateDescriptorSets(*device, &descriptorSetAllocateInfo, &descriptorSet);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = sampler;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfo.imageView = fontImageView;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.pImageInfo = &descriptorImageInfo;
		writeDescriptorSet.dstBinding = 0;

		vkUpdateDescriptorSets(*device, 1, &writeDescriptorSet, 0, nullptr);
	}

	void InitUIPipeline()
	{
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstant);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		vkCreatePipelineLayout(*device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

		ShaderModule vert(*device, "shader/UI.vert.spv");
		ShaderModule frag(*device, "shader/UI.frag.spv");

		std::vector<VkPipelineShaderStageCreateInfo> stages(2);
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].module = vert;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].pName = "main";

		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].module = frag;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].pName = "main";

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 1;
		VkVertexInputBindingDescription vertexInputBindingDescription = ImGuiVertexInputBindingDescription();
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = ImGuiVertexInputAttributeDescription();
		vertexInputState.pVertexBindingDescriptions = &vertexInputBindingDescription;
		vertexInputState.vertexAttributeDescriptionCount = vertexInputAttributeDescription.size();
		vertexInputState.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		VkViewport viewport = {};
		viewport.width = windowSize.width;
		viewport.height = windowSize.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		VkRect2D scissor = {};
		scissor.extent = windowSize;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = 1;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.blendEnable = VK_TRUE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendState.pAttachments = &colorBlendAttachmentState;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = stages.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		graphicsPipelineCreateInfo.pViewportState = &viewportState;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;

		vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
	}

	bool NewFrame()
	{
		ImGui::NewFrame();
		
		handleKey();

		//ImGui::ShowDemoWindow();

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		return true;
		if (drawData && drawData->CmdListsCount > 0) {
			if (drawData->TotalVtxCount != vertexCount || drawData->TotalIdxCount != indexCount) {
				return true;
			}
		}

		return false;
	}

	void updateBuffers()
	{
		ImDrawData* drawData = ImGui::GetDrawData();
		if (!drawData || drawData->CmdListsCount == 0) {
			return;
		}

		VkDeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

		if (drawData->TotalVtxCount != vertexCount) {
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = vertexBufferSize;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vkCreateBuffer(*device, &bufferCreateInfo, nullptr, &vertexBuffer);

			VkMemoryRequirements memoryRequirement;
			vkGetBufferMemoryRequirements(*device, vertexBuffer, &memoryRequirement);

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirement.size;
			memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkAllocateMemory(*device, &memoryAllocateInfo, nullptr, &vertexBufferMemory);
			vkBindBufferMemory(*device, vertexBuffer, vertexBufferMemory, 0);
			vertexCount = drawData->TotalVtxCount;
		}

		if (drawData->TotalIdxCount != indexCount) {
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = indexBufferSize;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vkCreateBuffer(*device, &bufferCreateInfo, nullptr, &indexBuffer);

			VkMemoryRequirements memoryRequirement;
			vkGetBufferMemoryRequirements(*device, indexBuffer, &memoryRequirement);

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirement.size;
			memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkAllocateMemory(*device, &memoryAllocateInfo, nullptr, &indexBufferMemory);
			vkBindBufferMemory(*device, indexBuffer, indexBufferMemory, 0);
			indexCount = drawData->TotalIdxCount;
		}
		ImDrawVert* vxt = new ImDrawVert[vertexCount];
		ImDrawIdx* idx = new ImDrawIdx[indexCount];
		ImDrawVert* vtxDst = &vxt[0];
		ImDrawIdx* idxDst = &idx[0];

		for (int n = 0; n < drawData->CmdListsCount; n++) {
			ImDrawList* cmdList = drawData->CmdLists[n];

			memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmdList->VtxBuffer.Size;
			idxDst += cmdList->IdxBuffer.Size;
		}
		void* data;
		vkMapMemory(*device, vertexBufferMemory, 0, vertexBufferSize, 0, &data);
		memcpy(data, vxt, vertexCount * sizeof(ImDrawVert));
		vkUnmapMemory(*device, vertexBufferMemory);
		data = nullptr;
		vkMapMemory(*device, indexBufferMemory, 0, indexBufferSize, 0, &data);
		memcpy(data, idx, indexCount * sizeof(ImDrawIdx));
		vkUnmapMemory(*device, indexBufferMemory);
	}

	void drawFrame(VkCommandBuffer& commandBuffer,VkFramebuffer& frameBuffer)
	{
		ImDrawData* drawData = ImGui::GetDrawData();

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = frameBuffer;
		VkRect2D renderArea = {};
		renderArea.extent = windowSize;
		renderPassBeginInfo.renderArea = renderArea;
		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { 1,1,1,1 };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		pushConstant.scale = glm::vec2(2.0f / drawData->DisplaySize.x, 2.0f / drawData->DisplaySize.y);
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pushConstant);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);
	}

	void handleKey() {
		//glfwGetMouseButton();
		ImGuiIO& io = ImGui::GetIO();


		if (io.MouseDown[0])
		{
			curPos = io.MousePos;
		}
		if (io.MouseDown[0])
		{
			pushConstant.translate.x += (io.MousePos-curPos).x;
			pushConstant.translate.y += (io.MousePos - curPos).y;
		}

		if (ImGui::SliderFloat("lightPosX", &lightPos[0], -1, 1))
		{
		}
		if (ImGui::SliderFloat("lightPosY", &lightPos[1], -1, 1))
		{
		}
		if (ImGui::SliderFloat("lightPosZ", &lightPos[2], -1, 1))
		{
		}
		if (ImGui::SliderFloat("lightIntensity", &lightIntensity, 0, 100))
		{
		}


		if (ImGui::ColorEdit3("lightColor",lightColorUI))
		{
			lightColor = glm::vec3(lightColorUI[0], lightColorUI[1], lightColorUI[2]);
		}
	}

	bool getWantKeyCapture() {
		return ImGui::GetIO().WantCaptureKeyboard;
	}

	void charPressed(uint32_t key) {
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(key);
	}
};