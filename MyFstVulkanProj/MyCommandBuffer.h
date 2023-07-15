#ifndef MY_COMMAND_BUFFER_H
#define MY_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>

class MyCommandBuffer
{
public:
	MyCommandBuffer(VkDevice device, uint32_t gfxQueueIdx,
		uint32_t pstQueueIdx, uint32_t cmdBufCount);
	~MyCommandBuffer();

	inline VkCommandBuffer* getCmdBufferPtr(int idx) { return &commandBuffers[idx]; }

	void recordCommandBuffer(int idx);

	inline void clearBuffer(int idx) { vkResetCommandBuffer(commandBuffers[idx], 0); }

	void startRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D swapChainExtent, int idx);

	void bindGFXPipeline(VkPipeline graphicsPipeline, VkExtent2D swapChainExtent, int idx);

	// TODO: size of buffers....HARD CODED now
	void bindVertexBuffers(VkBuffer vertexBuffer[1], VkDeviceSize offsets[1], int cmdBufferIdx);

	void draw(int idx, int vertexCount);

	void endRenderPass(VkRenderPass renderPass, int idx);

private:
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool commandPool;	// command buffers are allocated from command pool
	VkDevice device;
};

#endif // !MY_COMMAND_BUFFER_H