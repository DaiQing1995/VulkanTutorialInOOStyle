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

private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	MySwapchain* swapChain;
	VkDevice device;
};

#endif // !MY_FRAMEBUFFER_H