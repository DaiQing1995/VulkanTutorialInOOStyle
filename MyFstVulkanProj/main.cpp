/*
* This file followed tutorial of vulkan with some diy modifications.
*/
#include "MyVKInstance.h"
#include "MyVKPhyDev.h"
#include "MyLogicalDev.h"
#include "MySwapchain.h"
#include "MyGraphicsPipeline.h"
#include "MyFramebuffer.h"

#include <iostream>
#include <vector>

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
	}

	void mainLoop() {
		while (!window->should_close()) {
			window->poll_events();
		}
	}

	void cleanup() {
		delete framebuffers;
		delete gfxPipeline;
		delete swapChain;
		delete logDev;
		delete phyDev;
		delete ins;
		delete window;
	}

	MyWindow *window;
	MyVKInstance *ins;
	MyVKPhyDev* phyDev;
	MyLogicalDev* logDev;
	MySwapchain* swapChain;
	MyGraphicsPipeline* gfxPipeline;
	MyFramebuffer* framebuffers;
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