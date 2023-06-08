#include "MyWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
	MyWindow* myWindow = reinterpret_cast<MyWindow*>(glfwGetWindowUserPointer(window));
	myWindow->setFrameBufferResize(true);
}
/**
* Window creation related code, specific to glfw
*/
MyWindow::MyWindow(uint32_t win_width, uint32_t win_height) {
	glfwInit();
	m_height = win_height;
	m_width = win_width;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);

	// GLFW can't handle this->cbFunc, so store this pointer to GLFWwindow in such way
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

void MyWindow::getRequiredExtensions(uint32_t& glfwExtensionCount, const char*** glfwExtensions) const {
	*glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
}

MyWindow::~MyWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void MyWindow::getFrameBufferSize(int* width, int* height) const {
	glfwGetFramebufferSize(m_window, width, height);
}