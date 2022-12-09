#ifndef MY_FRAMEBUFFER_H
#define MY_FRAMEBUFFER_H

#include "MySwapchain.h"
#include "MyGraphicsPipeline.h"
#include <vector>

class MyFramebuffer
{

public:
	MyFramebuffer(MySwapchain *swapchain, MyGraphicsPipeline *pipeline);
	~MyFramebuffer();

	inline VkFramebuffer getFramebuffer(uint32_t index) { return swapChainFramebuffers[index]; }

private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	MySwapchain* swapChain;
	VkDevice device;
};

#endif // !MY_FRAMEBUFFER_H