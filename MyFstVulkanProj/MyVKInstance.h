#ifndef MY_VK_INSTANCE_H

#include "MyWindow.h"
#include <set>
#include <vector>
#include <string>

extern const bool enableValidationLayers;
extern const char* APP_NAME;
extern const std::vector<const char*> validationLayers;

class MyVKInstance {
public:

	void setupDebugMessenger();

	MyVKInstance(MyWindow *window);
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

	std::vector<const char*> getRequiredExtensions(MyWindow* window);

	VkInstance instance;
	std::set<std::string> extensions_set;
	VkDebugUtilsMessengerEXT debugMessenger;
};

#endif