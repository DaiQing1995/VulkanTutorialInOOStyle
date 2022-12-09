/*
* This file followed tutorial of vulkan with some diy modifications.
*/
#include "MyVKInstance.h"
#include "MyVKPhyDev.h"
#include "MyLogicalDev.h"
#include "MySwapchain.h"
#include "MyGraphicsPipeline.h"
#include "MyFramebuffer.h"
#include "MyCommandBuffer.h"

#include <iostream>

const char* APP_NAME = "Hello Triangle";

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:

	void initWindow() {
		window = new MyWindow();
	}

	void initVulkan() {
		// Init Instance
		ins = new MyVKInstance(window);
		ins->setupDebugMessenger();

		// Create surface
		VkSurfaceKHR surface = ins->createSurface();

		// create phycical device
		phyDev = new MyVKPhyDev(ins, surface);

		// create logical device
		logDev = new MyLogicalDev(phyDev->getPhyDev(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx());

		// create swapchain
		swapChain = new MySwapchain(window,
				phyDev->getSwapChainSupportDetails(),
				surface, logDev->getDevice(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx());

		// create Graphics Line
		gfxPipeline = new MyGraphicsPipeline(
				logDev->getDevice(),
				swapChain->getSwapChainExtent(),
				swapChain->getSwapChainImageFmt());

		framebuffers = new MyFramebuffer(swapChain, gfxPipeline);

		cmdbuffer = new MyCommandBuffer(logDev->getDevice(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx());

		createSyncObjects();
	}

	void mainLoop() {
		while (!window->should_close()) {
			window->poll_events();
			drawFrame();
		}
		// CPU leaves loops, it should wait operations on GPU done, then cleaning.
		vkDeviceWaitIdle(logDev->getDevice());
	}

	void cleanup() {
		vkDestroySemaphore(logDev->getDevice(), imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logDev->getDevice(), renderFinishedSemaphore, nullptr);
		vkDestroyFence(logDev->getDevice(), inFlightFence, nullptr);
		delete cmdbuffer;
		delete framebuffers;
		delete gfxPipeline;
		delete swapChain;
		delete logDev;
		delete phyDev;
		delete ins;
		delete window;
	}

	/**
	* Drawing logic:
	* 1. Wait for the previous frame to finish
	* 2. Acquire an image from the swap chain
	* 3. Record a command buffer which draws to scene on that image
	* 4. submit the recorded command buffer
	* 5. present the swap chain image
	*/
	void drawFrame() {
		// 1. wait
		vkWaitForFences(logDev->getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(logDev->getDevice(), 1, &inFlightFence);
		uint32_t imageIndex;
		// 2. acquire
		// after acquire, imageAvailable Semaphore signaled(Inside GPU, CPU continues), 
		// indicates drawing starts 
		// imageIndex is the framebuffer index inside swapchain framebuffers vector
		vkAcquireNextImageKHR(logDev->getDevice(), swapChain->getSwapChain(), UINT64_MAX,
			imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		// 3. record
		cmdbuffer->clearBuffer();
		cmdbuffer->recordCommandBuffer();
		cmdbuffer->startRenderPass(gfxPipeline->getRenderPass(), framebuffers->getFramebuffer(imageIndex),
			swapChain->getSwapChainExtent());
		cmdbuffer->bindGFXPipeline(gfxPipeline->getPipeline(), swapChain->getSwapChainExtent());
		cmdbuffer->draw();
		cmdbuffer->endRenderPass(gfxPipeline->getRenderPass());

		// 4. submit
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
		// Actually this one(Top of Pipe) could ensure the imagelayout has changed from
		// viewable(swapchain) to writable(pipeline), but that is subpass dependency's job
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;	// just theoretically assume pipeline
													// could not finished, add this barrier
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdbuffer->getCmdBufferPtr();
		
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(logDev->getGFXQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		// 5. present
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { swapChain->getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;	// used for every swapchain checking
		vkQueuePresentKHR(logDev->getPresentQueue(), &presentInfo);
	}

	void createSyncObjects() {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// create fence with signaled state.
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (vkCreateSemaphore(logDev->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore)
			!= VK_SUCCESS ||
			vkCreateSemaphore(logDev->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore)
			!= VK_SUCCESS ||
			vkCreateFence(logDev->getDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphores");
		}
	}

	MyWindow *window;
	MyVKInstance *ins;
	MyVKPhyDev* phyDev;
	MyLogicalDev* logDev;
	MySwapchain* swapChain;
	MyGraphicsPipeline* gfxPipeline;
	MyFramebuffer* framebuffers;
	MyCommandBuffer* cmdbuffer;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;
};

int main() {
	HelloTriangleApplication app{};

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}