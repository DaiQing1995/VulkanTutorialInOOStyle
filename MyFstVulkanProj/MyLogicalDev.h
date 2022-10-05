#ifndef MY_LOGICAL_DEV_H
#define MY_LOGICAL_DEV_H

#include "vulkan/vulkan.h"
#include <vector>

extern const bool enableValidationLayers;
extern const std::vector<const char*> validationLayers;
extern const std::vector<const char*> deviceExtensions;

class MyLogicalDev
{
public:
	MyLogicalDev(VkPhysicalDevice phyDev, uint32_t graphicsQueueFamIdx,
		uint32_t presentQueueFamIdx);
	~MyLogicalDev();

	inline VkDevice getDevice() { return device; }

private:
	VkDevice device;
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkQueue graphicsQueue;
	VkQueue presentQueue;
};

#endif // !MY_LOGICAL_DEV_H