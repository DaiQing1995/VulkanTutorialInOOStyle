#ifndef MY_QUEUE_FAMILY_H

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

class MyQueueFamily
{
public:

	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

private:
};

#endif // !MY_QUEUE_FAMILY_H