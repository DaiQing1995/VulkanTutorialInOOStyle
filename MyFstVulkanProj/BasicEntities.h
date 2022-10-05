#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

/*
* Swapchain Extention should be supported by physical devices,
* after checking its availability, there are still 3 things need
* to record:
* 1. Surface capability(number for images in swapchain, min/max
* width and height of images),
* 2. Surface Formats(pixel format, color space)
* 3. Available presentation modes
*/
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

/**
* QueueFamily used for graphics process and present.
*/
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;	// take charge of Draw
	std::optional<uint32_t> presentFamily;	// take charge of Present to Display

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};