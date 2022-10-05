#ifndef MY_VK_PHY_DEV_H
#define MY_VK_PHY_DEV_H

#include <vulkan/vulkan.h>
#include <vector>
#include "MyVKInstance.h"
#include "MyQueueFamily.h"
#include "BasicEntities.h"

extern const std::vector<const char*> deviceExtensions;

/**
* My physical Device records VKPhysicalDevice and Structures direct to
* displays.
*/
class MyVKPhyDev
{
public:
	MyVKPhyDev(MyVKInstance* ins, VkSurfaceKHR surface);
	~MyVKPhyDev();

	inline VkPhysicalDevice getPhyDev() { return physicalDevice; }
	inline uint32_t getGraphicsQueueFamilyIdx() { return indices.graphicsFamily.value(); }
	inline uint32_t getPresentQueueFamilyIdx() { return indices.presentFamily.value(); }
	inline const SwapChainSupportDetails* getSwapChainSupportDetails() const { return &swapChainSupportDetails; }

private:
	void pickPhysicalDevice(VkSurfaceKHR surface);
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	VkPhysicalDevice physicalDevice;
	MyVKInstance *instance;
	QueueFamilyIndices indices;
	VkSurfaceKHR surface;
	SwapChainSupportDetails swapChainSupportDetails;
};

#endif // !MY_VK_PHY_DEV_H