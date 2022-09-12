#include "MyLogicalDev.h"
#include <stdexcept>

MyLogicalDev::MyLogicalDev(VkPhysicalDevice phyDev, uint32_t queueFamIdx)
{
	/**
	* The Queue will be automatically created along with the logical
	* device. First need to set required queue info.
	*/
	VkDeviceQueueCreateInfo queueCreateInfo{};

	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueFamIdx;
	/*
	* Queue's count is limited, while multi queue is not that necessary,
	* we can use command buffers on multi thread and submit them to this
	* queue in main thread.
	*/
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;
	/**
	* Only for compatibility with old vulkan version, following works are needed,
	* they have already been set in instance layer, and works for VkDevice.
	*/
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	
	if (vkCreateDevice(phyDev, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	/**
	* The 3rd param indicates the queue index we want, since we only specify
	* 1 queue, so use 0 to get the first queue.
	*/
	vkGetDeviceQueue(device, queueFamIdx, 0, &graphicsQueue);
}

MyLogicalDev::~MyLogicalDev()
{
	vkDestroyDevice(device, nullptr);
	// For VkQueue which will be implicitly cleaned up when the device is destroyed.
}
