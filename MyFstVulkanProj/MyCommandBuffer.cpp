#include "MyCommandBuffer.h"
#include <stdexcept>

MyCommandBuffer::MyCommandBuffer(VkDevice device,
		uint32_t gfxQueueIdx, uint32_t pstQueueIdx):device(device) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	/**
	* 2 types for flags:
	* - VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Command buffers are rerecoreded with new
	* Commands very often (May change memory allocation behavior)
	* 
	* - VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be
	* rerecorded individually, without this flag they all have to be reset together
	* 
	* We need record a command buffer every frame, so need to reset and rerecord over it.
	* Not the case of TRANSIENT but 1 used frequently.
	*/
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	/**
	* 1 pool, 1 type queue
	* Each command pool can only allocate command buffers that are submitted on a single
	* type of queue.
	*/
	poolInfo.queueFamilyIndex = gfxQueueIdx;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	// TODO: Create Command Buffer
}

MyCommandBuffer::~MyCommandBuffer() {
	vkDestroyCommandPool(device, commandPool, nullptr);
}