#ifndef MY_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	inline bool should_close() {
		return glfwWindowShouldClose(m_window);
	}
	
	void getRequiredExtensions(uint32_t& glfwExtensionCount, const char*** glfwExtensions);

private:
	uint32_t m_height;
	uint32_t m_width;
	GLFWwindow* m_window;
};

#endif // !MY_WINDOW_H