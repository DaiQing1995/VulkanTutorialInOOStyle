#ifndef MY_VK_PHY_DEV_H
#define MY_VK_PHY_DEV_H

#include <vulkan/vulkan.h>
#include "MyQueueFamily.h"

class MyVKPhyDev
{
public:
	MyVKPhyDev(VkInstance ins, VkSurfaceKHR surface);
	~MyVKPhyDev();

	inline VkPhysicalDevice getPhyDev() { return physicalDevice; }
	inline uint32_t getQueueFamilyIdx() { return indices.graphicsFamily.value(); }

private:
	void pickPhysicalDevice(VkSurfaceKHR surface);
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

	VkPhysicalDevice physicalDevice;
	VkInstance instance;
	QueueFamilyIndices indices;
};

#endif // !MY_VK_PHY_DEV_H