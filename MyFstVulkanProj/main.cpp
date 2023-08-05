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
#include "MyBufferHelper.h"

#include "Application.h"

#include "TriangleResource.h"
#include <iostream>

const int MAX_FRAMES_IN_FLIGHT = 2;

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

class ApplicationEntry {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	ApplicationEntry(Application* app) :app(app){}

private:

	// Create Window for Win System
	void initWindow() {
		window = new MyWindow();
	}

	// Vulkan environment settings
	void initVulkan() {
		// Init Instance
		ins = new MyVKInstance(window, APP_NAME);
		ins->setupDebugMessenger();

		// Create surface
		ins->createSurface();

		// Create phycical device
		phyDev = new MyVKPhyDev(ins, ins->getSurface());

		// Create logical device
		logDev = new MyLogicalDev(phyDev->getPhyDev(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx());

		// Create swapchain
		swapChain = new MySwapchain(window,
				phyDev->getSwapChainSupportDetails(),
				ins->getSurface(), logDev->getDevice(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx());

		// Create Graphics Line
		gfxPipeline = new MyGraphicsPipeline(
				logDev->getDevice(),
				swapChain->getSwapChainExtent(),
				swapChain->getSwapChainImageFmt(),
				app);

		// Create framebuffer
		framebuffers = new MyFramebuffer(swapChain, gfxPipeline);

		// Create Command buffer
		cmdbuffer = new MyCommandBuffer(logDev->getDevice(),
				phyDev->getGraphicsQueueFamilyIdx(),
				phyDev->getPresentQueueFamilyIdx(), MAX_FRAMES_IN_FLIGHT);

		// Create objects for sync
		createSyncObjects();

		currentFrame = 0;

		// Create Buffer Helper
		bufferHelper = new BufferHelper(logDev->getDevice(), phyDev->getPhyDev());

		// Create App needed vertex buffer
		app->createVertexBuffer(bufferHelper);
	}

	void recreateSwapChain() {
		// consider the minimization condition, w/h comes to 0
		int width = 0, height = 0;
		window->getFrameBufferSize(&width, &height);
		while (width == 0 || height == 0) {
			window->getFrameBufferSize(&width, &height);
			window->wait_events();
		}

		vkDeviceWaitIdle(logDev->getDevice());
		delete framebuffers;
		delete swapChain;
		// Create swapchain (New size would be queried during creation)
		swapChain = new MySwapchain(window,
			phyDev->getSwapChainSupportDetails(),
			ins->getSurface(), logDev->getDevice(),
			phyDev->getGraphicsQueueFamilyIdx(),
			phyDev->getPresentQueueFamilyIdx());
		// Create framebuffer
		framebuffers = new MyFramebuffer(swapChain, gfxPipeline);
		// NOTE: here does not create pipeline, which has extent info
		// difference between old one and new one.
	}

	// Drawing
	void mainLoop() {
		while (!window->should_close()) {
			window->poll_events();
			drawFrame();
		}
		// CPU leaves loops, it should wait operations on GPU done, then cleaning.
		vkDeviceWaitIdle(logDev->getDevice());
	}

	void cleanup() {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			vkDestroySemaphore(logDev->getDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(logDev->getDevice(), renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(logDev->getDevice(), inFlightFences[i], nullptr);
		}
		delete bufferHelper;
		delete cmdbuffer;
		delete framebuffers;
		delete gfxPipeline;
		delete swapChain;
		delete logDev;
		delete phyDev;
		delete ins;
		delete window;
		delete app;
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
		// 1. wait 1 signal for inFlightFence
		vkWaitForFences(logDev->getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		// 2. acquire image index
		// after acquire, imageAvailable Semaphore signaled(Inside GPU, CPU continues), 
		// imageIndex is the VkImage index inside swapchain's swapChainImages vector
		VkResult result = vkAcquireNextImageKHR(logDev->getDevice(), swapChain->getSwapChain(), UINT64_MAX,
			imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		// FEAT: add recreate
		/**
		* VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the
		* 	surface and can no longer be used for rendering. Usually happens after
		* 	a window resize.
		* VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present
		*	to the surface, but the surface properties are no longer matched exactly.
		*/
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		// recover the signal
		vkResetFences(logDev->getDevice(), 1, &inFlightFences[currentFrame]);

		// 3. record
		cmdbuffer->clearBuffer(currentFrame);
		cmdbuffer->recordCommandBuffer(currentFrame);
		cmdbuffer->startRenderPass(gfxPipeline->getRenderPass(),
			framebuffers->getFramebuffer(imageIndex),
			swapChain->getSwapChainExtent(), currentFrame);
		cmdbuffer->bindGFXPipeline(gfxPipeline->getPipeline(),
			swapChain->getSwapChainExtent(), currentFrame);

		std::vector<VkBuffer> vertexBufferVec;
		app->getVertexBuffers(vertexBufferVec);
		std::vector<VkDeviceSize> vertexBufferOffsets;
		app->getVertexBuffersOffsets(vertexBufferOffsets);
		cmdbuffer->bindVertexBuffers(vertexBufferVec.data(), vertexBufferOffsets.data(), currentFrame);

		cmdbuffer->draw(currentFrame, app->getVertexDrawingSize());
		cmdbuffer->endRenderPass(gfxPipeline->getRenderPass(), currentFrame);

		// 4. submit
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
		// Actually this one(Top of Pipe) could ensure the imagelayout has changed from
		// viewable(swapchain) to writable(pipeline), but that is subpass dependency's job
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;	// just theoretically assume pipeline
													// could not finished since it is signaled
													// during vkAcquireNextImageKHR
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdbuffer->getCmdBufferPtr(currentFrame);

		// let it signal renderFinish semaphore after executed
		VkSemaphore renderFinsied[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = renderFinsied;

		// Signal inFlightFence after execution, then CPU can reuse the command buffer.
		if (vkQueueSubmit(logDev->getGFXQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		// 5. present
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		// presents must wait finished
		presentInfo.pWaitSemaphores = renderFinsied;

		VkSwapchainKHR swapchains[] = { swapChain->getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;	// used for every swapchain checking
		result = vkQueuePresentKHR(logDev->getPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
				|| window->shouldFrameBufferResize()) {
			window->setFrameBufferResize(false);
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	/**
	* Create 2 semaphores:
	*	- imageAvailable: represents display finished, and the image can be reused
	*   - renderFinished: represents GPU finished job, time for display.
	* Create 1 Fence:
	*	- Command submit and get execute, Let CPU wait GPU.
	*/
	void createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		// GPU inside
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		// CPU wait GPU
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// create fence with signaled state.
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (vkCreateSemaphore(logDev->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i])
				!= VK_SUCCESS ||
				vkCreateSemaphore(logDev->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i])
				!= VK_SUCCESS ||
				vkCreateFence(logDev->getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create semaphores");
			}
		}
	}

	Application* app;
	MyWindow *window;
	MyVKInstance *ins;
	MyVKPhyDev* phyDev;
	MyLogicalDev* logDev;

	MySwapchain* swapChain;
	MyGraphicsPipeline* gfxPipeline;
	MyFramebuffer* framebuffers;

	uint32_t currentFrame;
	MyCommandBuffer* cmdbuffer;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	BufferHelper *bufferHelper;
};

int main() {
	TriangleResourceIf* triangle = new TriangleResourceIf;
	ApplicationEntry app(triangle);

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