#include "MyLogicalDev.h"
#include <stdexcept>
#include <vector>
#include <set>

MyLogicalDev::MyLogicalDev(VkPhysicalDevice phyDev, uint32_t graphicsQueueFamIdx, uint32_t presentQueueFamIdx)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	// Graphics and Present Queue Family may be same.
	std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamIdx, presentQueueFamIdx };
	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		/**
		* The Queue will be automatically created along with the logical
		* device. First need to set required queue info.
		*/
		VkDeviceQueueCreateInfo queueCreateInfo{};

		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		/*
		* Queue's count is limited, while multi queue is not that necessary,
		* we can use command buffers on multi thread and submit them to this
		* queue in main thread.
		*/
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
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
	vkGetDeviceQueue(device, graphicsQueueFamIdx, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueFamIdx, 0, &presentQueue);
}

MyLogicalDev::~MyLogicalDev()
{
	vkDestroyDevice(device, nullptr);
	// For VkQueue which will be implicitly cleaned up when the device is destroyed.
}
