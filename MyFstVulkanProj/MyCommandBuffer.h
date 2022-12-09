#ifndef MY_COMMAND_BUFFER_H
#define MY_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>

class MyCommandBuffer
{
public:
	MyCommandBuffer(VkDevice device, uint32_t gfxQueueIdx, uint32_t pstQueueIdx);
	~MyCommandBuffer();

private:
	VkCommandBuffer commandBuffer;
	VkCommandPool commandPool;	// command buffers are allocated from command pool
	VkDevice device;
};

#endif // !MY_COMMAND_BUFFER_H