#include "MyVKPhyDev.h"
#include "MyConfig.h"

#ifdef INIT_PHYSICAL_DEVICE_DEBUG
#include <iostream>
#endif // INIT_PHYSICAL_DEVICE_DEBUG

#include <stdexcept>
#include <vector>
#include <map>
#include <set>

MyVKPhyDev::MyVKPhyDev(MyVKInstance *ins, VkSurfaceKHR surface)
	:instance(ins), physicalDevice(VK_NULL_HANDLE), surface(surface)
{
	pickPhysicalDevice(surface);
}

MyVKPhyDev::~MyVKPhyDev() {
	// VkPhysicalDevice will be destroyed implicitly
	// after instance destroyed. No need to destroy it.
}

bool MyVKPhyDev::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
												availableExtensions.data());

	/**
	* Available extensions is saved as vector, n times traverse is inevitable.
	* Use erase for fast handling.
	*/
	std::set<std::string> requiredExtensions(deviceExtensions.begin(),
												deviceExtensions.end());
#ifdef INIT_PHYSICAL_DEVICE_DEBUG
	for (auto& iter : requiredExtensions) {
		std::cout << "[PhyDev] required Ext: " << iter << std::endl;
	}
#endif // INIT_PHYSICAL_DEVICE_DEBUG

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	bool extensionSupported = requiredExtensions.empty();
	if (!extensionSupported)
		return false;

	// After check swapchain is supported, need to require the swapchain's
	// capability for futher determination.
	bool swapChainAdequate = false;
	if (extensionSupported) {
		// Check Swapchain support is OK,there is at least one format can
		// be used for swapchain image, and at least on mode for diaplay.
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() &&
				!swapChainSupport.presentModes.empty();
	}

	return swapChainAdequate;
}

SwapChainSupportDetails MyVKPhyDev::querySwapChainSupport(VkPhysicalDevice device) {

	// Get capabilities, need use Physical Device and Surface
	// which are the 2 core components of the swapchain.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
		&swapChainSupportDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device,
		surface, &formatCount, nullptr);

	if (formatCount != 0) {
		swapChainSupportDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
			&formatCount, swapChainSupportDetails.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
		&presentModeCount, nullptr);

	if (presentModeCount != 0) {
		swapChainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
			&presentModeCount, swapChainSupportDetails.presentModes.data());
	}

	return swapChainSupportDetails;
}

/**
* Check if the device is suitable
*/
bool MyVKPhyDev::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	indices = MyQueueFamily::findQueueFamilies(device, surface);

	return  checkDeviceExtensionSupport(device)
		// Mark : following judge is direct but not elegent,
		// now everything is explicit, folling line is of no use.
		// deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader && indices.isComplete();
}

void MyVKPhyDev::pickPhysicalDevice(VkSurfaceKHR surface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	else {
#ifdef INIT_PHYSICAL_DEVICE_DEBUG
		std::cout << "[PhyDev] physical device count: " << deviceCount << std::endl;
#endif //INIT_PHYSICAL_DEVICE_DEBUG
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, devices.data());
	for (const auto& device : devices) {
		if (isDeviceSuitable(device, surface)) {
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}