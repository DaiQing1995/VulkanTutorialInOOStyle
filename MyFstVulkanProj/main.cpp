/*
* This file followed tutorial of vulkan with some diy modifications.
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t DEFAULT_WIN_WIDTH = 800;
const uint32_t DEFAULT_WIN_HEIGHT = 600;

class MyWindow {

public:

	MyWindow(uint32_t win_width = DEFAULT_WIN_WIDTH, uint32_t win_height = DEFAULT_WIN_HEIGHT) {
		glfwInit();
		m_height = win_height;
		m_width = win_width;
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);
	}

	inline void poll_events() {
		glfwPollEvents();
	}
	inline bool should_close() {
		return glfwWindowShouldClose(m_window);
	}
	inline void close_window() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
private:
	uint32_t m_height;
	uint32_t m_width;
	GLFWwindow* m_window;
};

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

	}

	void mainLoop() {
		while (!window->should_close()) {
			window->poll_events();
		}
	}

	void cleanup() {
		window->close_window();
		delete window;
	}

	MyWindow *window;
};

int main() {
	HelloTriangleApplication app;

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