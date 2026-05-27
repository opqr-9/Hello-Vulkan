#pragma once
#include "Start.h"

class ShaderModule
{
	VkShaderModule handle = VK_NULL_HANDLE;
public:
	operator VkShaderModule& ()
	{
		return handle;
	}

	ShaderModule(VkDevice& device, const char* filePath)
	{
		std::ifstream shaderfile(filePath, std::ios::ate | std::ios::binary);
		size_t codeSize = size_t(shaderfile.tellg());
		std::vector<uint32_t> binaries(codeSize / 4);
		shaderfile.seekg(0);
		shaderfile.read(reinterpret_cast<char*>(binaries.data()), codeSize);
		shaderfile.close();

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = codeSize;
		shaderModuleCreateInfo.pCode = binaries.data();
		vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &handle);
	}
};