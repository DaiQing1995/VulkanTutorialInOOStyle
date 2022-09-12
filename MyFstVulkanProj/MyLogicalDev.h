#ifndef MY_LOGICAL_DEV_H

#include "vulkan/vulkan.h"
#include <vector>

extern const bool enableValidationLayers;
extern const std::vector<const char*> validationLayers;

class MyLogicalDev
{
public:
	MyLogicalDev(VkPhysicalDevice phyDev, uint32_t queueFamIdx);
	~MyLogicalDev();

private:
	VkDevice device;
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkQueue graphicsQueue;
};

#endif // !MY_LOGICAL_DEV_H