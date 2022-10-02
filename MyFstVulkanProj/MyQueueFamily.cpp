#include "MyQueueFamily.h"
#include "MyConfig.h"

#include <vector>

#ifdef INIT_FAMILY_QUEUE_DEBUG
#include <iostream>
#endif // INIT_FAMILY_QUEUE_DEBUG


/**
* Find the index of family queue which support Graphics cmd
*/ 
QueueFamilyIndices MyQueueFamily::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    bool graphicsFound = false;
    bool presentFound = false;
    for (const auto& queueFamily : queueFamilies) {
        if (!graphicsFound && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            indices.graphicsFamily = i;
            graphicsFound = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (!presentFound && presentSupport) {
            indices.presentFamily = i;
            presentFound = true;
        }
        if (graphicsFound && presentFound) {
#ifdef INIT_FAMILY_QUEUE_DEBUG
            std::cout << "[Queue] Graphics queue family: "
                << indices.graphicsFamily.value() << std::endl;
            std::cout << "[Queue] Present queue family: "
                << indices.presentFamily.value() << std::endl;
#endif
            break;
        }
        i ++;
    }

    return indices;
}
