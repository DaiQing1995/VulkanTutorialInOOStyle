#ifndef MY_VK_INSTANCE_H
#define MY_VK_INSTANCE_H

#include "MyWindow.h"
#include <set>
#include <vector>
#include <string>

extern const bool enableValidationLayers;
extern const std::vector<const char*> validationLayers;

class MyVKInstance {
public:

	void setupDebugMessenger();

	inline VkInstance getInstance() {
		return instance;
	}

	inline VkSurfaceKHR getSurface() {
		return surface;
	}

	VkSurfaceKHR createSurface();

	MyVKInstance(const MyWindow* window, const char *appName);
	~MyVKInstance();

private:
	
	// debug utils
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// wrappers
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	// utils
	void check_ext(const char** exts, const int cnt);
	bool checkValidationLayerSupport();

	// Required extensions for instance
	std::vector<const char*> getRequiredExtensions(const MyWindow* window);

	const MyWindow* window;
	VkInstance instance;
	std::set<std::string> extensions_set;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
};

#endif