#ifndef MY_VK_PHY_DEV_H

#include <vulkan/vulkan.h>

class MyVKPhyDev
{
public:
	MyVKPhyDev(VkInstance ins);
	~MyVKPhyDev();

private:
	void pickPhysicalDevice();

	VkPhysicalDevice physicalDevice;
	VkInstance instance;
};

#endif // !MY_VK_PHY_DEV_H