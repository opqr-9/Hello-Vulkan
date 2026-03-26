#pragma once
#include "Start.h"
#include "OBJParser.h"

VkExtent2D windowSize = { 1280,720 };

#define DefineHandleTypeOperator operator decltype(handle)() const { return handle; }
#define DefineAddressFunction const decltype(handle)* Address() const { return &handle; }
#define MAX_FRAMES_IN_FLIGHT 2

struct GameObject {
	glm::vec3 position = { 0.0f,0.0f,0.0f };
	glm::vec3 rotation = { 0.0f,0.0f,0.0f };
	glm::vec3 scale = { 1.0f,1.0f,1.0f };

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	std::vector<VkDescriptorSet> descriptorSets;

	glm::mat4 GetModelMartix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		return model;
	}
};

const int MAX_OBJECTS = 3;
std::vector<GameObject> gameObjects;

//const glm::vec3 cameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
const glm::vec3 cameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
const glm::vec3 lightPos = glm::vec3(4.0f, 0.0f, 4.0f);
//const glm::vec3 lightPos = glm::vec3(4.0f, 0.0f, 4.0f);

//const std::string objFilePath = "textures/viking_room.obj";
//const std::string textureFilePath = "textures/viking_room.png";

/*

const std::string objFilePath = "textures/textures2/Rock.obj";
const std::string textureFilePath = "textures/textures2/Rock_BaseColor.png";
const std::string metallicFilePath = "textures/textures2/Rock_Metallic.png";
const std::string normalFilePath = "textures/textures2/Rock_Normal.png";
const std::string roughnessFilePath = "textures/textures2/Rock_Roughness.png";

const std::string objFilePath = "textures/textures3/moss rock 01.obj";
const std::string textureFilePath = "textures/textures3/Moss rock 01 color 4k.png";
const std::string metallicFilePath = "textures/textures3/metallic.png";
const std::string normalFilePath = "textures/textures3/Moss rock 01 normal 4k.png";
const std::string roughnessFilePath = "textures/textures3/Moss rock 01 roughness 4k.png";
*/

const std::string objFilePath = "textures/textures1/Stone.obj";
const std::string textureFilePath = "textures/textures1/stone_low+_Stone_low_1_Emissive.png";
//const std::string textureFilePath = "textures/textures1/stone_low+_Stone_low_1_BaseColor.png";
const std::string metallicFilePath = "textures/textures1/stone_low+_Stone_low_1_Metallic.png";
const std::string normalFilePath = "textures/textures1/stone_low+_Stone_low_1_Normal.png";
const std::string roughnessFilePath = "textures/textures1/stone_low+_Stone_low_1_Roughness.png";

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;

	bool operator==(const Vertex& other) const {
		return pos == other.pos && texCoord == other.texCoord && normal == other.normal && tangent == other.tangent;
	}

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, tangent);

		return attributeDescriptions;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			// 计算每个键的哈希
			size_t h1 = hash<glm::vec3>{}(vertex.pos);
			size_t h2 = hash<glm::vec2>{}(vertex.texCoord);
			size_t h3 = hash<glm::vec3>{}(vertex.normal);
			size_t h4 = hash<glm::vec3>{}(vertex.tangent);

			size_t seed = h1;
			seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}

struct UniformBufferObject {
	alignas(16)glm::vec3 lightPos;
	alignas(16)glm::vec3 cameraPos;
	alignas(16)glm::mat4 model;
	alignas(16)glm::mat4 view;
	alignas(16)glm::mat4 proj;
};

std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

std::vector<Vertex> vertices = {
	/*
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}

	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	*/
};

std::vector<uint32_t> indices = {
	/*
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
	*/
};

class GraphicsBase 
{
	GraphicsBase() = default;
	GraphicsBase(GraphicsBase&&) = delete;
	~GraphicsBase(){}

	VkResult GetQueueFamilyIndices(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyPropertyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

		for (uint32_t i = 0; i < queueFamilyPropertyCount; i++)
		{
			VkBool32 supportPresentation=false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation);
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
				supportPresentation)
			{
				graphicsQueueIndex = computeQueueIndex = presentationQueueIndex = i;
				return VK_SUCCESS;
			}
		}
		return VK_RESULT_MAX_ENUM;
	}

	void AddLayersOrExtensions(std::vector<const char*>& container, const char* name)
	{
		for (auto& i : container)
		{
			if (!strcmp(i, name))
			{
				return;
			}
		}
		container.emplace_back(name);
	}

	void GetSurfaceFormat()
	{
		uint32_t surfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
		availableSurfaceFormat.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, availableSurfaceFormat.data());
	}

	void SetSurfacFormat(VkSurfaceFormatKHR surfaceFormat)
	{
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	}

	void GetSurfacePresentMode()
	{
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	}

	void CreateSwapchain()
	{
		vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);

		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		swapchainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
		swapchainImageViews.resize(swapchainImageCount);
		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = swapchainCreateInfo.imageFormat;
			imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			imageViewCreateInfo.image = swapchainImages[i];
			vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]);
		}
	}

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
	}

	void CreateBuffer(VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer &buffer,VkDeviceMemory &bufferMemory)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer);

		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

		vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory);

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	VkCommandBuffer beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = 1;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		return commandBuffer;
	}

	void endSingleTimeCommands(VkCommandBuffer commandBuffer) 
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);
	}

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, uint32_t height, uint32_t width)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;

		copyRegion.imageOffset = {0,0,0};
		copyRegion.imageExtent = { width,height,1 };

		vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	void CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	}

	void UpdateUniformBuffer(uint32_t currentFrame)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//ubo.proj = glm::ortho<float>(-2, 2, -2, 2, 0.1f, 1000.0f);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), swapchainCreateInfo.imageExtent.width / (float)swapchainCreateInfo.imageExtent.height, 0.1f, 1000.0f);
		proj[1][1] *= -1;

		for (uint32_t i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i].rotation.z += 0.001f;

			glm::mat4 initialRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 model = gameObjects[i].GetModelMartix() * initialRotation;
			//model = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			UniformBufferObject ubo = { lightPos,cameraPos,model,view,proj };

			memcpy(gameObjects[i].uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
		}

		return;

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
		ubo.view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//ubo.proj = glm::ortho<float>(-2, 2, -2, 2, 0.1f, 1000.0f);
		ubo.proj = glm::perspective(glm::radians(45.0f), swapchainCreateInfo.imageExtent.width / (float)swapchainCreateInfo.imageExtent.height, 0.1f, 1000.0f);
		ubo.proj[1][1] *= -1;
		ubo.lightPos = lightPos;
		ubo.cameraPos = cameraPos;

		memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
	}

	void CreateImage(int width,int height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usage,VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		vkCreateImage(device, &imageInfo, nullptr, &image);

		VkMemoryRequirements memoryRequirement = {};
		vkGetImageMemoryRequirements(device, image, &memoryRequirement);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirement.size;
		memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirement.memoryTypeBits, properties);

		vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory);

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView; 
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view!");
		}
		//vkCreateImageView(device, &viewInfo, nullptr, &imageView);

		return imageView;
	}

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) 
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}


		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}



	void CreateTextureImage(std::string filepath, VkImage& image, VkDeviceMemory& imageMemory, VkFormat format)
	{
		int texWidth = 0, texHeight = 0, texChannels = 0;
		stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, imageSize);
		vkUnmapMemory(device, stagingBufferMemory);

		stbi_image_free(pixels);

		CreateImage(texWidth, texHeight, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);

		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

public:
	VkClearValue clearColor = {};

	static GraphicsBase base;

	std::vector<const char*> instanceLayers;
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> deviceExtensions;

	uint32_t apiVersion = VK_API_VERSION_1_0;

	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;

	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice;
	std::vector<VkPhysicalDevice> availablePhysicalDevices;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	uint32_t graphicsQueueIndex;
	uint32_t computeQueueIndex;
	uint32_t presentationQueueIndex;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkQueue computeQueue;
	VkDevice device;

	std::vector<VkSurfaceFormatKHR> availableSurfaceFormat;
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

	VkRenderPass renderPass;

	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;

	VkPipeline pipeline;

	std::vector<VkFramebuffer> framebuffer;

	VkCommandPool commandPool;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage metallicImage;
	VkDeviceMemory metallicImageMemory;
	VkImageView metallicImageView;
	VkSampler metallicSampler;

	VkImage normalImage;
	VkDeviceMemory normalImageMemory;
	VkImageView normalImageView;
	VkSampler normalSampler;

	VkImage roughnessImage;
	VkDeviceMemory roughnessImageMemory;
	VkImageView roughnessImageView;
	VkSampler roughnessSampler;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkBuffer lightBuffer;

	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory lightBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	uint32_t currentFrame = 0;

	std::vector<VkCommandBuffer> drawCommandBuffers;

	std::vector<VkFence> fences_OrdersIsOver;
	std::vector<VkSemaphore> semaphores_ImageIsAvailable;
	std::vector<VkSemaphore> semaphores_RenderingIsOver;
	
	void AddInstanceLayer(const char* name)
	{
		AddLayersOrExtensions(instanceLayers, name);
	}
	void AddInstanceExtension(const char* name)
	{
		AddLayersOrExtensions(instanceExtensions, name);
	}
	void AddDeviceExtension(const char* name)
	{
		AddLayersOrExtensions(deviceExtensions, name);
	}



	void UseLatestApiVersion()
	{
		if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
		{
			vkEnumerateInstanceVersion(&apiVersion);
		}
	}

	void CreateVkInstance()
	{
		uint32_t extensionCount;
		const char** extensionsNames = glfwGetRequiredInstanceExtensions(&extensionCount);
		for (uint32_t i = 0; i < extensionCount; i++)
		{
			AddInstanceExtension(extensionsNames[i]);
		}
		AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		AddInstanceLayer("VK_LAYER_KHRONOS_validation");
		AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.apiVersion = apiVersion;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = instanceLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
		instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
			std::cout << "[ graphicsBase ] ERROR\nFailed to create a vulkan instance!\nError code: {}\n";
			return;
		}
	}

	void CreateDeBugMessenger()
	{
		static PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessengerCallback = [](
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData)->VkBool32 {
				std::cout << pCallbackData->pMessage << std::endl;
				return VK_FALSE;
		};

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
		debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCreateInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugUtilsMessengerCreateInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

		vkCreateDebugUtilsMessenger(instance, &debugUtilsMessengerCreateInfo, nullptr, &debugMessenger);
	}

	void CreateSurface(GLFWwindow* pWindow)
	{
		glfwCreateWindowSurface(instance, pWindow, nullptr, &surface);
	}

	void PickPhysicalDevice()
	{
		uint32_t physicalDeviceCount;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		availablePhysicalDevices.resize(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, availablePhysicalDevices.data());

		for (uint32_t i = 0; i < availablePhysicalDevices.size(); i++)
		{
			if (!GetQueueFamilyIndices(availablePhysicalDevices[i]))
			{
				physicalDevice = availablePhysicalDevices[i];
				return;
			}
		}
	}

	void CreateDevice()
	{
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo deviceQueueCreateInfos = {};
		deviceQueueCreateInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfos.queueFamilyIndex = graphicsQueueIndex;
		deviceQueueCreateInfos.queueCount = 1;
		deviceQueueCreateInfos.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos;
		deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

		vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

		vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(device, computeQueueIndex, 0, &computeQueue);
		vkGetDeviceQueue(device, presentationQueueIndex, 0, &presentationQueue);

		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
	}

	void InitSwapchain()
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = std::min(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.maxImageCount);
		swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.clipped = VK_TRUE;

		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		SetSurfacFormat({ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });

		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

		CreateSwapchain();
	}

	void CreateRenderPass()
	{
		VkAttachmentDescription imageAttachment = {};
		imageAttachment.format = swapchainCreateInfo.imageFormat;
		imageAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		imageAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		imageAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		imageAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		imageAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		imageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		std::vector<VkAttachmentDescription> attachments = { imageAttachment,depthAttachment };

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCreateInfo.pAttachments = attachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;
		vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
	}

	void CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
		textureSamplerLayoutBinding.binding = 1;
		textureSamplerLayoutBinding.descriptorCount = 1;
		textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureSamplerLayoutBinding.pImmutableSamplers = nullptr;
		textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding metallicSamplerLayoutBinding{};
		metallicSamplerLayoutBinding.binding = 2;
		metallicSamplerLayoutBinding.descriptorCount = 1;
		metallicSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		metallicSamplerLayoutBinding.pImmutableSamplers = nullptr;
		metallicSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding normalSamplerLayoutBinding{};
		normalSamplerLayoutBinding.binding = 3;
		normalSamplerLayoutBinding.descriptorCount = 1;
		normalSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalSamplerLayoutBinding.pImmutableSamplers = nullptr;
		normalSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding roughnessSamplerLayoutBinding{};
		roughnessSamplerLayoutBinding.binding = 4;
		roughnessSamplerLayoutBinding.descriptorCount = 1;
		roughnessSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		roughnessSamplerLayoutBinding.pImmutableSamplers = nullptr;
		roughnessSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::vector<VkDescriptorSetLayoutBinding> bindings = { 
			uboLayoutBinding, 
			textureSamplerLayoutBinding,
			metallicSamplerLayoutBinding,
			normalSamplerLayoutBinding,
			roughnessSamplerLayoutBinding
		};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
	}

	void CreateGraphicsPipeline()
	{
		VkShaderModuleCreateInfo vertshaderModuleCreateInfo = {};
		vertshaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		std::ifstream vertfile("shader/FirstTriangle.vert.spv", std::ios::ate | std::ios::binary);
		size_t codeSize = size_t(vertfile.tellg());
		std::vector<uint32_t> binaries(codeSize / 4);
		vertfile.seekg(0);
		vertfile.read(reinterpret_cast<char*>(binaries.data()), codeSize);
		vertfile.close();
		vertshaderModuleCreateInfo.codeSize = codeSize;
		vertshaderModuleCreateInfo.pCode = binaries.data();
		VkShaderModule vert;
		vkCreateShaderModule(device, &vertshaderModuleCreateInfo, nullptr, &vert);

		VkShaderModuleCreateInfo fragshaderModuleCreateInfo = {};
		fragshaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		std::ifstream fragfile("shader/FirstTriangle.frag.spv", std::ios::ate | std::ios::binary);
		codeSize = size_t(fragfile.tellg());
		binaries.clear();
		binaries.resize(codeSize / 4);
		fragfile.seekg(0);
		fragfile.read(reinterpret_cast<char*>(binaries.data()), codeSize);
		fragfile.close();
		VkShaderModule frag;
		fragshaderModuleCreateInfo.codeSize = codeSize;
		fragshaderModuleCreateInfo.pCode = binaries.data();
		vkCreateShaderModule(device, &fragshaderModuleCreateInfo, nullptr, &frag);

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {};
		pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pipelineShaderStageCreateInfo[0].module = vert;
		pipelineShaderStageCreateInfo[0].pName = "main";

		pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pipelineShaderStageCreateInfo[1].module = frag;
		pipelineShaderStageCreateInfo[1].pName = "main";

		CreatePipelineLayout();

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
		std::vector<VkVertexInputBindingDescription> vkVertexInputBindingDescription = {
			Vertex::getBindingDescription()
		};
		std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescription(Vertex::getAttributeDescriptions());
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vkVertexInputBindingDescription.size();
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescription.data();
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vkVertexInputAttributeDescription.size();
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescription.data();

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = {};
		pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		VkViewport viewport = {};
		viewport.width = float(windowSize.width);
		viewport.height = float(windowSize.height);
		viewport.maxDepth = 1.f;
		pipelineViewportStateCreateInfo.pViewports = &viewport;
		pipelineViewportStateCreateInfo.scissorCount = 1;
		VkRect2D scissor = {};
		scissor.extent = windowSize;
		pipelineViewportStateCreateInfo.pScissors = &scissor;
		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		//pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
		pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
		graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = &pipelineTessellationStateCreateInfo;
		graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
	}

	void CreateFramebuffers()
	{
		framebuffer.resize(swapchainImages.size());
		
		for (uint32_t i = 0; i < swapchainImages.size(); i++)
		{
			std::vector<VkImageView> attachments = { swapchainImageViews[i],depthImageView };
			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = windowSize.width;
			framebufferCreateInfo.height = windowSize.height;
			framebufferCreateInfo.layers = 1;
			vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer[i]);
		}
	}

	void CreateCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = graphicsQueueIndex;
		vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
	}

	void CreateDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();
		CreateImage(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void CreateUniformBuffers() 
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		for (uint32_t i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i].uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			gameObjects[i].uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
			gameObjects[i].uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
			{
				CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					gameObjects[i].uniformBuffers[j], gameObjects[i].uniformBuffersMemory[j]);

				vkMapMemory(device, gameObjects[i].uniformBuffersMemory[j], 0, bufferSize, 0, &gameObjects[i].uniformBuffersMapped[j]);
			}
		}

		return;

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void CreateDescriptorPool()
	{
		std::vector<VkDescriptorPoolSize> poolSizes(2);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_OBJECTS * MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_OBJECTS * MAX_FRAMES_IN_FLIGHT * 4);

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_OBJECTS * MAX_FRAMES_IN_FLIGHT);

		vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	}

	void CreateDescriptorSets()
	{
		for (uint32_t i = 0; i < gameObjects.size(); i++)
		{
			std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
			allocInfo.pSetLayouts = layouts.data();

			gameObjects[i].descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
			vkAllocateDescriptorSets(device, &allocInfo, gameObjects[i].descriptorSets.data());
			for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
				//std::cout << i << j << std::endl;
				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = gameObjects[i].uniformBuffers[j];
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);
				/*
				std::vector<VkDescriptorImageInfo> imageInfos = {
					{textureSampler,textureImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
					{metallicSampler,metallicImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
					{normalSampler,normalImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
					{roughnessSampler,roughnessImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
				};

				std::vector<VkWriteDescriptorSet> descriptorWrites(2);

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = descriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = imageInfos.size();
				descriptorWrites[1].pImageInfo = imageInfos.data();
				*/

				VkDescriptorImageInfo textureImageInfo = {};
				textureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				textureImageInfo.imageView = textureImageView;
				textureImageInfo.sampler = textureSampler;

				VkDescriptorImageInfo metallicImageInfo = {};
				metallicImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				metallicImageInfo.imageView = metallicImageView;
				metallicImageInfo.sampler = metallicSampler;

				VkDescriptorImageInfo normalImageInfo = {};
				normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				normalImageInfo.imageView = normalImageView;
				normalImageInfo.sampler = normalSampler;

				VkDescriptorImageInfo roughnessImageInfo = {};
				roughnessImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				roughnessImageInfo.imageView = roughnessImageView;
				roughnessImageInfo.sampler = roughnessSampler;

				std::vector<VkWriteDescriptorSet> descriptorWrites(5);
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = gameObjects[i].descriptorSets[j];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = gameObjects[i].descriptorSets[j];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pImageInfo = &textureImageInfo;

				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = gameObjects[i].descriptorSets[j];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pImageInfo = &metallicImageInfo;

				descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[3].dstSet = gameObjects[i].descriptorSets[j];
				descriptorWrites[3].dstBinding = 3;
				descriptorWrites[3].dstArrayElement = 0;
				descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[3].descriptorCount = 1;
				descriptorWrites[3].pImageInfo = &normalImageInfo;

				descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[4].dstSet = gameObjects[i].descriptorSets[j];
				descriptorWrites[4].dstBinding = 4;
				descriptorWrites[4].dstArrayElement = 0;
				descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[4].descriptorCount = 1;
				descriptorWrites[4].pImageInfo = &roughnessImageInfo;

				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		}

		return;

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);
			/*
			std::vector<VkDescriptorImageInfo> imageInfos = {
				{textureSampler,textureImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
				{metallicSampler,metallicImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
				{normalSampler,normalImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
				{roughnessSampler,roughnessImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites(2);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = imageInfos.size();
			descriptorWrites[1].pImageInfo = imageInfos.data();
			*/

			VkDescriptorImageInfo textureImageInfo = {};
			textureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			textureImageInfo.imageView = textureImageView;
			textureImageInfo.sampler = textureSampler;

			VkDescriptorImageInfo metallicImageInfo = {};
			metallicImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			metallicImageInfo.imageView = metallicImageView;
			metallicImageInfo.sampler = metallicSampler;

			VkDescriptorImageInfo normalImageInfo = {};
			normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			normalImageInfo.imageView = normalImageView;
			normalImageInfo.sampler = normalSampler;

			VkDescriptorImageInfo roughnessImageInfo = {};
			roughnessImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			roughnessImageInfo.imageView = roughnessImageView;
			roughnessImageInfo.sampler = roughnessSampler;

			std::vector<VkWriteDescriptorSet> descriptorWrites(5);
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &textureImageInfo;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = descriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &metallicImageInfo;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = descriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &normalImageInfo;

			descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[4].dstSet = descriptorSets[i];
			descriptorWrites[4].dstBinding = 4;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pImageInfo = &roughnessImageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void CreateTextureImages()
	{
		CreateTextureImage(textureFilePath, textureImage, textureImageMemory, VK_FORMAT_R8G8B8A8_SRGB);
		CreateTextureImage(metallicFilePath, metallicImage, metallicImageMemory, VK_FORMAT_R8G8B8A8_UNORM);
		CreateTextureImage(normalFilePath, normalImage, normalImageMemory, VK_FORMAT_R8G8B8A8_UNORM);
		CreateTextureImage(roughnessFilePath, roughnessImage, roughnessImageMemory, VK_FORMAT_R8G8B8A8_UNORM);
	}

	void CreateTextureImageView()
	{
		textureImageView = CreateImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
		metallicImageView = CreateImageView(metallicImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
		normalImageView = CreateImageView(normalImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
		roughnessImageView = CreateImageView(roughnessImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler);
		metallicSampler = normalSampler = roughnessSampler = textureSampler;
	}

	void LoadModel()
	{
		OBJ obj = OBJParser::Instance().Parser(objFilePath);
		std::cout << obj.v.size() << std::endl;
		std::cout << obj.vt.size() << std::endl;
		std::cout << obj.vn.size() << std::endl;
		std::cout << obj.f.size() << std::endl;
		
		vertices.resize(obj.f.size() * 3);
		indices.resize(obj.f.size() * 3);

		Vertex stageVertex;
		glm::vec3 stageTangent;

		for (long long i = 0; i < obj.f.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)
				{
					glm::vec3 ba = obj.v[obj.f[i][1][0] - 1] - obj.v[obj.f[i][0][0] - 1];
					glm::vec3 cb = obj.v[obj.f[i][2][0] - 1] - obj.v[obj.f[i][1][0] - 1];
					glm::vec2 bauv = obj.vt[obj.f[i][1][1] - 1] - obj.vt[obj.f[i][0][1] - 1];
					bauv.y = - bauv.y;
					glm::vec2 cbuv = obj.vt[obj.f[i][2][1] - 1] - obj.vt[obj.f[i][1][1] - 1];
					cbuv.y = - cbuv.y;
					stageTangent = (ba * cbuv.y - cb * bauv.y) / (bauv.x * cbuv.y - cbuv.x * bauv.y);
				}
				stageVertex.pos = obj.v[obj.f[i][j][0] - 1];
				stageVertex.texCoord = obj.vt[obj.f[i][j][1] - 1];
				stageVertex.texCoord.y = 1.0f - stageVertex.texCoord.y;
				stageVertex.normal = obj.vn[obj.f[i][j][2] - 1];
				stageVertex.tangent = stageTangent;
				if (uniqueVertices.count(stageVertex) == 0)
				{
					uniqueVertices[stageVertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(stageVertex);
				}
				indices.push_back(uniqueVertices[stageVertex]);
			}
		}
	}

	void SetUpGameObjects()
	{
		gameObjects.resize(MAX_OBJECTS);
		gameObjects[0].position = { 0.0f, 0.0f, 0.0f };
		gameObjects[0].rotation = { 0.0f, 0.0f, 0.0f };
		gameObjects[0].scale = { 1.0f, 1.0f, 1.0f };

		gameObjects[1].position = { 0.0f, 2.0f, 0.0f };
		gameObjects[1].rotation = { 0.0f, 0.0f, 0.0f };
		gameObjects[1].scale = { 1.25f, 1.25f, 1.25f };

		gameObjects[2].position = { 0.0f, -2.0f, 0.0f };
		gameObjects[2].rotation = { 0.0f, 0.0f, 0.0f };
		gameObjects[2].scale = { 0.75f, 0.75f, 0.75f };
	}

	void CreateVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void CreateIndexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	/*
	void CreateLightBuffer()
	{
		VkDeviceSize bufferSize = sizeof(light);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, &light, (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, lightBuffer, lightBufferMemory);

		CopyBuffer(stagingBuffer, lightBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	*/

	void CreateCommandBuffers()
	{
		drawCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = drawCommandBuffers.size();
		vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, drawCommandBuffers.data());
	}

	void CreateSyncObjects()
	{
		fences_OrdersIsOver.resize(MAX_FRAMES_IN_FLIGHT);
		semaphores_ImageIsAvailable.resize(MAX_FRAMES_IN_FLIGHT);
		semaphores_RenderingIsOver.resize(MAX_FRAMES_IN_FLIGHT);

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (int i = 0; i < fences_OrdersIsOver.size(); i++)
		{
			vkCreateFence(device, &fenceCreateInfo, nullptr, &fences_OrdersIsOver[i]);

			vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores_ImageIsAvailable[i]);
			vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores_RenderingIsOver[i]);
		}
	}

	void DrawFrame()
	{
		clearColor.color = { 1.f, 1.f, 1.f, 1.f };
		vkWaitForFences(device, 1, &fences_OrdersIsOver[currentFrame], false, UINT64_MAX);
		vkResetFences(device, 1, &fences_OrdersIsOver[currentFrame]);

		uint32_t currentImageIndex;
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphores_ImageIsAvailable[currentFrame], VK_NULL_HANDLE, &currentImageIndex);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(drawCommandBuffers[currentFrame], &commandBufferBeginInfo);

		VkRenderPassBeginInfo renderpassBeginInfo = {};
		renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassBeginInfo.renderPass = renderPass;
		renderpassBeginInfo.framebuffer = framebuffer[currentImageIndex];
		VkRect2D renderArea = {};
		renderArea.extent = windowSize;
		renderpassBeginInfo.renderArea = renderArea;
		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { clearColor.color };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderpassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderpassBeginInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(drawCommandBuffers[currentFrame], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(drawCommandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(drawCommandBuffers[currentFrame], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		for (uint32_t i = 0; i < gameObjects.size(); i++)
		{
			vkCmdBindDescriptorSets(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, 
				&gameObjects[i].descriptorSets[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(drawCommandBuffers[currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		}

		//vkCmdBindDescriptorSets(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
		//vkCmdDrawIndexed(drawCommandBuffers[currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		//vkCmdDrawIndexed(drawCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		//vkCmdDraw(drawCommandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(drawCommandBuffers[currentFrame]);

		vkEndCommandBuffer(drawCommandBuffers[currentFrame]);

		UpdateUniformBuffer(currentFrame);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &drawCommandBuffers[currentFrame];
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &semaphores_ImageIsAvailable[currentFrame];
		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.pWaitDstStageMask = &pipelineStageFlags;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &semaphores_RenderingIsOver[currentFrame];
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences_OrdersIsOver[currentFrame]);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &semaphores_RenderingIsOver[currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &currentImageIndex;
		vkQueuePresentKHR(presentationQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void CleanUp()
	{
		CleanUpSwapchain();

		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);

		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);
	}

	void CleanUpSwapchain()
	{

	}

	void RecreateSwapchain()
	{
		VkSurfaceCapabilitiesKHR surfacecapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfacecapabilities);

		swapchainCreateInfo.imageExtent = surfacecapabilities.currentExtent;
		swapchainCreateInfo.minImageCount = std::min(surfacecapabilities.minImageCount + 1, surfacecapabilities.maxImageCount);
		swapchainCreateInfo.preTransform = surfacecapabilities.currentTransform;
		swapchainCreateInfo.oldSwapchain = swapchain;

		vkQueueWaitIdle(presentationQueue);
		if (presentationQueue != graphicsQueue)
		{
			vkQueueWaitIdle(graphicsQueue);
		}

		for (uint32_t i = 0; i < swapchainImageViews.size(); i++)
		{
			vkDestroyImageView(device, swapchainImageViews[i], nullptr);
		}

		CreateSwapchain();
	}

};

/*
class ShaderModule
{
	VkShaderModule handle=VK_NULL_HANDLE;

public:
	DefineHandleTypeOperator
	DefineAddressFunction

	ShaderModule(const char* filepath)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		std::ifstream shaderfile(filepath, std::ios::ate | std::ios::binary);
		size_t codeSize = size_t(shaderfile.tellg());
		std::vector<uint32_t> binaries(codeSize / 4);
		shaderfile.seekg(0);
		shaderfile.read(reinterpret_cast<char*>(binaries.data()), codeSize);
		shaderfile.close();
		shaderModuleCreateInfo.codeSize = codeSize;
		shaderModuleCreateInfo.pCode = binaries.data();
		vkCreateShaderModule(GraphicsBase::Base().Device(), &shaderModuleCreateInfo, nullptr, &handle);
	}

};

class Fence
{
	VkFence handle = VK_NULL_HANDLE;

public:
	DefineHandleTypeOperator
	DefineAddressFunction

	Fence()
	{
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		vkCreateFence(GraphicsBase::Base().Device(), &fenceCreateInfo, nullptr, &handle);
	}

};

class Semaphore
{
	VkSemaphore handle=VK_NULL_HANDLE;

public:
	DefineHandleTypeOperator
	DefineAddressFunction

	Semaphore()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(GraphicsBase::Base().Device(), &semaphoreCreateInfo, nullptr, &handle);
	}

};

class CommandPool
{
	VkCommandPool handle = VK_NULL_HANDLE;

public:
	DefineHandleTypeOperator
	DefineAddressFunction

	CommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = GraphicsBase::Base().GraphicsQueueIndex();
		vkCreateCommandPool(GraphicsBase::Base().Device(), &commandPoolCreateInfo, nullptr, &handle);
	}
};

class CommandBuffer 
{
	VkCommandBuffer handle = VK_NULL_HANDLE;

public:
	DefineHandleTypeOperator
	DefineAddressFunction

	CommandBuffer(CommandPool commandPool)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(GraphicsBase::Base().Device(), &commandBufferAllocateInfo, &handle);
	}
};
*/

inline GraphicsBase GraphicsBase::base;
