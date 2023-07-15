#ifndef TRIANGLE_SOURCE_H
#define TRIANGLE_SOURCE_H

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <array>
#include "MyBufferHelper.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

const std::vector<Vertex> Triangle_Vertices = {
	{{0.0f, -0.5f}, {0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
};

class TriangleResourceIf
{
public:
	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	/**
	* Create Vertex Buffer binding GPU
	*/
	static void createTriangleVertexBuffer(BufferHelper* bufHelper);

	static VkBuffer vertexBuffer;

private:
	TriangleResourceIf() {}
	~TriangleResourceIf() {}
};
#endif