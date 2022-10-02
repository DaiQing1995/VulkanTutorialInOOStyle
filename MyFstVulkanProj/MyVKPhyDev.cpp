#include "MyVKPhyDev.h"

#include <stdexcept>
#include <vector>
#include <map>

MyVKPhyDev::MyVKPhyDev(VkInstance ins, VkSurfaceKHR surface) {
	instance = ins;
	physicalDevice = VK_NULL_HANDLE;
	pickPhysicalDevice(surface);
}

MyVKPhyDev::~MyVKPhyDev() {
	// VkPhysicalDevice will be destroyed implicitly
	// after instance destroyed. No need to destroy it.
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

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader && indices.isComplete();
}

void MyVKPhyDev::pickPhysicalDevice(VkSurfaceKHR surface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
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
