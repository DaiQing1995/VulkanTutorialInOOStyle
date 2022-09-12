/*
* This file followed tutorial of vulkan with some diy modifications.
*/
#include "MyVKInstance.h"
#include <iostream>
#include <vector>

const char* APP_NAME = "Hello Triangle";

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
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
		ins = new MyVKInstance(window);
		ins->setupDebugMessenger();
	}

	void mainLoop() {
		while (!window->should_close()) {
			window->poll_events();
		}
	}

	void cleanup() {
		delete ins;
		delete window;
	}

	MyWindow *window;
	MyVKInstance *ins;
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