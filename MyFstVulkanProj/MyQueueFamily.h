#ifndef MY_QUEUE_FAMILY_H
#define MY_QUEUE_FAMILY_H

#include "BasicEntities.h"

class MyQueueFamily
{
public:
	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:

};

#endif // !MY_QUEUE_FAMILY_H