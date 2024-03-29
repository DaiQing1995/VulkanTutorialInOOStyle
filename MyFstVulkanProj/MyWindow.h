#ifndef MY_WINDOW_H
#define MY_WINDOW_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

const uint32_t DEFAULT_WIN_WIDTH = 800;
const uint32_t DEFAULT_WIN_HEIGHT = 600;
/**
* Window creation related code, specific to glfw
*/
class MyWindow {
public:
	MyWindow(uint32_t win_width = DEFAULT_WIN_WIDTH, uint32_t win_height = DEFAULT_WIN_HEIGHT);

	~MyWindow();

	inline void poll_events() {
		glfwPollEvents();
	}

	inline void wait_events() {
		glfwPollEvents();
	}

	inline GLFWwindow* getGLFWWindow() const {
		return m_window;
	}

	inline bool should_close() {
		return glfwWindowShouldClose(m_window);
	}

	inline bool shouldFrameBufferResize() {
		return framebufferdResized;
	}

	inline void setFrameBufferResize(bool shouldResize) {
		framebufferdResized = shouldResize;
	}
	
	void getRequiredExtensions(uint32_t& glfwExtensionCount, const char*** glfwExtensions) const;
	void getFrameBufferSize(int* width, int* height) const;

private:
	uint32_t m_height;
	uint32_t m_width;
	GLFWwindow* m_window;
	bool framebufferdResized = false;
};

#endif // !MY_WINDOW_H