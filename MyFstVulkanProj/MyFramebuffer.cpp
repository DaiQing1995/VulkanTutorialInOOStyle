#include "MyFramebuffer.h"
#include <stdexcept>

/**
* Framebuffer just represents the color attachment. can not be regarded as a buffer and being used
* for mapping, drawing, etc.
* The drawing operation would happen direcly on VkImage. FB is just a simbol.
*/
MyFramebuffer::MyFramebuffer(MySwapchain *swapchain, MyGraphicsPipeline* pipeline)
	:swapChain(swapchain), device(swapchain->device){

	swapChainFramebuffers.resize(swapChain->swapChainImageViews.size());
	for (size_t i = 0; i < swapchain->swapChainImageViews.size(); ++i) {
		VkImageView attachments[] = {
			swapchain->swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = pipeline->renderPass;	// framebuffer needs to be compatible with
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchain->swapChainExtent.width;
		framebufferInfo.height = swapchain->swapChainExtent.height;
		framebufferInfo.layers = 1;	// number of layers in image array

		if (vkCreateFramebuffer(swapchain->device, &framebufferInfo, nullptr,
				&swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

MyFramebuffer::~MyFramebuffer() {
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}