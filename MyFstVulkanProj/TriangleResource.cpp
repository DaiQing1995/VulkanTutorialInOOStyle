#include "TriangleResource.h"

VkBuffer TriangleResourceIf::vertexBuffer = VK_NULL_HANDLE;

VkVertexInputBindingDescription TriangleResourceIf::getBindingDescription() {

	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	/**
	* VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
	* VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
	*
	* Instanced rendering is not being used.
	*/
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return  bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> TriangleResourceIf::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
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
	return attributeDescriptions;
}

void TriangleResourceIf::createTriangleVertexBuffer(BufferHelper* bufHelper)
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