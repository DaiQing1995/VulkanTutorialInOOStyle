#ifndef APPLICATION_H
#define APPLICATION_H

#include <vulkan/vulkan.h>
#include "MyBufferHelper.h"

class Application
{
public:
	virtual void vertexInputSetting4Pipeline(VkPipelineVertexInputStateCreateInfo* info)=0;
	virtual void createVertexBuffer(BufferHelper* bufHelper)=0;
	virtual void getVertexBuffers(std::vector<VkBuffer> &containers)=0;
	virtual void getVertexBuffersOffsets(std::vector<VkDeviceSize>& containers)=0;
	virtual unsigned int getVertexDrawingSize()=0;
};

#endif