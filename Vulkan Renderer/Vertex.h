#pragma once
#include "Start.h"

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