#include "MyCommandBuffer.h"
#include <stdexcept>

MyCommandBuffer::MyCommandBuffer(VkDevice device,
		uint32_t gfxQueueIdx, uint32_t pstQueueIdx,
		uint32_t cmdBufCount):device(device) {
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

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	/**
	* Primiary command buffer can be submitted to a queue for execution, can not 
	* be called from other command buffers.
	* 
	* Secondary command buffer can not be submitted directly, but can be called
	* from primary command buffer.
	*/
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = cmdBufCount;

	commandBuffers.resize(cmdBufCount);
	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

MyCommandBuffer::~MyCommandBuffer() {
	vkDestroyCommandPool(device, commandPool, nullptr);
}

void MyCommandBuffer::recordCommandBuffer(int idx) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	/**
	* VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: command will be rerecorded right
	* after execution.
	* VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command
	* buffer that will be entirely within a single render pass.
	* VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: Command buffer can be resubmitted
	* while it is also already pending execution.
	*/
	beginInfo.flags = 0;
	// only used for secondary, inherit primary command buffer
	beginInfo.pInheritanceInfo = nullptr;

	// If the command buffer was already recorded once, vkBeginCommandBuffer will
	// implicitly reset it.
	if (vkBeginCommandBuffer(commandBuffers[idx], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}
}

/**
* Specify the render pass used during this command buffer submit.
* And and the VkFramebuffer for swapchain image description.
*/
void MyCommandBuffer::startRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D swapChainExtent, int idx) {

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	// For reload pipeline line setting
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	/**
	* VK_SUBPASS_CONTENTS_INLINE: Render Pass command embedded in primary command buffer, no secondary
	* cmdbuf.
	* VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: Render Pass command will be executed from
	* secondary cmdbuf.
	*/
	vkCmdBeginRenderPass(commandBuffers[idx], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void MyCommandBuffer::bindVertexBuffers(std::vector<VkBuffer> vertexBuffers,
					std::vector<VkDeviceSize> offsets, int cmdBufferIdx) {

	vkCmdBindVertexBuffers(commandBuffers[cmdBufferIdx], 0, 1, vertexBuffers.data(), offsets.data());
}

void MyCommandBuffer::bindGFXPipeline(VkPipeline gfxPipeline,
		VkExtent2D swapChainExtent, int idx) {

	vkCmdBindPipeline(commandBuffers[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline);

	// TODO: Pipeline dynamic settings, which is strong relates to pipeline sets, make
	// it flexible in the future. While for now, the usecase is not clear, delay this task.
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffers[idx], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffers[idx], 0, 1, &scissor);
}

void MyCommandBuffer::draw(int idx, int vertexCount) {
	// vertexCount, instanceCount, firstVertex, firstInstance
	// vertex now is statically set in shader.
	vkCmdDraw(commandBuffers[idx], vertexCount, 1, 0, 0);
}

void MyCommandBuffer::endRenderPass(VkRenderPass renderPass, int idx) {
	// End render pass
	vkCmdEndRenderPass(commandBuffers[idx]);

	// Finish recording
	if (vkEndCommandBuffer(commandBuffers[idx]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}