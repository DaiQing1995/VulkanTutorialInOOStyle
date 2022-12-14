#ifndef MY_COMMAND_BUFFER_H
#define MY_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>

class MyCommandBuffer
{
public:
	MyCommandBuffer(VkDevice device, uint32_t gfxQueueIdx, uint32_t pstQueueIdx);
	~MyCommandBuffer();

	inline VkCommandBuffer* getCmdBufferPtr() { return &commandBuffer; }

	void recordCommandBuffer();
	inline void clearBuffer() { vkResetCommandBuffer(commandBuffer, 0); }
	void startRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D swapChainExtent);
	void bindGFXPipeline(VkPipeline graphicsPipeline, VkExtent2D swapChainExtent);
	void draw();
	void endRenderPass(VkRenderPass renderPass);

private:
	VkCommandBuffer commandBuffer;
	VkCommandPool commandPool;	// command buffers are allocated from command pool
	VkDevice device;
};

#endif // !MY_COMMAND_BUFFER_H