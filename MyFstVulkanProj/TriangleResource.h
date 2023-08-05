#ifndef TRIANGLE_SOURCE_H
#define TRIANGLE_SOURCE_H

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <array>
#include "MyBufferHelper.h"
#include "Application.h"

extern const char* APP_NAME;

class TriangleResourceIf : public Application
{
public:
	/**
	* Create Vertex Buffer binding GPU
	*/
	void createVertexBuffer(BufferHelper* bufHelper);

	/**
	* Used as callback in pipeline creation
	*/
	void vertexInputSetting4Pipeline(VkPipelineVertexInputStateCreateInfo* info);

	void getVertexBuffers(std::vector<VkBuffer> &containers);
	void getVertexBuffersOffsets(std::vector<VkDeviceSize>& containers);
	unsigned int getVertexDrawingSize();

	TriangleResourceIf();
	~TriangleResourceIf();
private:
	VkVertexInputBindingDescription bindingDescription;
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;
	VkBuffer vertexBuffer;
};
#endif