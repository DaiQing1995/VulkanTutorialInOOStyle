#include "TriangleResource.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

const std::vector<Vertex> Triangle_Vertices = {
	{{0.0f, -0.5f}, {0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
};

const char* APP_NAME = "Hello Triangle";

void TriangleResourceIf::createVertexBuffer(BufferHelper* bufHelper)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(Triangle_Vertices[0]) * Triangle_Vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	// Buffer could be owned by a specific queue family or
	// being shared between multipile at the same time.
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufHelper->createBufferWithMemory(bufferInfo, vertexBuffer);
	bufHelper->copyCPUData2Memory(vertexBuffer, Triangle_Vertices.data(), bufferInfo.size);
}

void TriangleResourceIf::vertexInputSetting4Pipeline(VkPipelineVertexInputStateCreateInfo* info) {
	info->vertexBindingDescriptionCount = 1;
	info->pVertexBindingDescriptions = &bindingDescription;
	info->vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	info->pVertexAttributeDescriptions = attributeDescriptions.data();
}

void TriangleResourceIf::getVertexBuffers(std::vector<VkBuffer> &containers) {
	containers.push_back(vertexBuffer);
}

/**
* Vertex Buffer Offsets needs to be align with Vertex Buffer Arrtibute
* Format size. In this case, it is VK_FORMAT_R32G32B32_SFLOAT.
*/
void TriangleResourceIf::getVertexBuffersOffsets(std::vector<VkDeviceSize>& containers) {
	containers.push_back(0);
}

unsigned int TriangleResourceIf::getVertexDrawingSize() {
	return static_cast<unsigned int>(Triangle_Vertices.size());
}

TriangleResourceIf::TriangleResourceIf() {
	vertexBuffer = VK_NULL_HANDLE;
	// 1. Binding: spacing between data and whether data is per-vertex or per-instance
	bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	/**
	* VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
	* VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
	*
	* Instanced rendering is not being used.
	*/
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// 2. Attribute: type of attributes passed to the vertex shader, offset
	attributeDescriptions = {};
	// which binding the per-vertex data comes. The buffer is GPU created.
	attributeDescriptions[0].binding = 0;
	// reference the location directive of the input in vertex sbader.
	attributeDescriptions[0].location = 0;
	// do not relate to color format, just use it for 2 float
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	// the start point in data.
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	// color attribute uses the same buffer, so binding buffer sets to 0 as well.
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);
}

TriangleResourceIf::~TriangleResourceIf() {

}