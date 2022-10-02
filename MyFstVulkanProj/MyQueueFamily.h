#ifndef MY_QUEUE_FAMILY_H
#define MY_QUEUE_FAMILY_H

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;	// take charge of Draw
	std::optional<uint32_t> presentFamily;	// take charge of Present to Display

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class MyQueueFamily
{
public:

	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
};

#endif // !MY_QUEUE_FAMILY_H