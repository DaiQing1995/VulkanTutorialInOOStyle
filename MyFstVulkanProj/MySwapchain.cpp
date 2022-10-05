#include "MySwapchain.h"

#include <cstdint>
#include <limits>
#include <algorithm>
#include <stdexcept>

MySwapchain::MySwapchain(const MyWindow *window,
		const SwapChainSupportDetails* swapchainSD,
		VkSurfaceKHR surface, VkDevice device,
		uint32_t gfxIdx, uint32_t pstIdx):
			device(device), window(window) {

	// get determined format, present mode, and extent
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSD->formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSD->presentModes);
	VkExtent2D extent = chooseSwapExtent(swapchainSD->capabilities);

	// record format and extent info
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	// driver handles minImageCount images for swapchain, which may results
	// in program waiting too long. So add 1 for compensation.
	uint32_t imageCount = swapchainSD->capabilities.minImageCount + 1;
	
	// if maxImageCoun equals to 0, which means no limit
	if (swapchainSD->capabilities.maxImageCount > 0 &&
		imageCount > swapchainSD->capabilities.maxImageCount) {
		imageCount = swapchainSD->capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	// layers of this image containing
	createInfo.imageArrayLayers = 1;
	// This surface will be rendered directly
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { gfxIdx, pstIdx };
	if (gfxIdx != pstIdx) {
		// Images can be used across multiple queue families.
		// No explicit transfer needed. Just easy.
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		// Image ownership is owned by one queue family, best performace.
		// Ownership transfer needs explicit transfer.
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	// A certain transform to swapchain image can be set, eg: rotation, flip
	createInfo.preTransform = swapchainSD->capabilities.currentTransform;
	// Whether to blend this window with other window in Window System
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	// We do not care pixels that are obscured by other window.
	// Unless we care the output pixels from memory not only one screen,
	// we shoould set to flase to ban clip.
	createInfo.clipped = VK_TRUE;
	// Swapchain may need recreate when window is resized or...
	// When it happens, oldSwapchin may be used for providing infos for newer
	// swapchain.
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain");
	}

	// Acquire swapchain images
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());

	// Initialize swapchain image views
	swapChainImageViews.resize(imageCount);
	for (size_t i = 0; i < swapChainImages.size(); ++i) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		// components allow program to swizzle color channels, here uses default
		// eg: mapp all channels to red, it would get a monochrome texture
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// This image is used for color target, neither use mipmap nor multiple layers
		// PS: multiple layers is used for stereographic 3D application, Imageviews for
		// left eye and right eye are different
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

MySwapchain::~MySwapchain() {
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

/**
* VkSurfaceFormatKHR contains a format and a colorSpace member.
* VK_FORMAT_B8G8R8A8_SRGB => B8G8R8A8 represents format and bpp
*							 SRGB represents color space
* SRGB is preferred due to its accurate preceivced colors.
*/
VkSurfaceFormatKHR MySwapchain::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

/**
*
* Present modes represent actual conditions for showing images to screen:
*
* - IMMEDIATE: Image submitted by application would be transfered to screen
* directly. So tearing may occur.
*
* - FIFO: Swapchain is a queue, display takes image from the front buffer,
* rendered image is added to the rear. If the queue is full, program waits.
*
* - FIFO_RELAXED: The only difference compared to the above one is that when
* Application is late, queue is empty, it will sent the first coming image
* to display. Like Immediate mode, it may results in tearing.
*
* - MAILBOX: if queue is full, application does not being blocked but update
* the queue with latest contents. Tearing is avoided. Energy consuming but
* looks good.
*
* NOTE: FIFO is guaranteed to be available.
*/
VkPresentModeKHR MySwapchain::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

/**
* Extent of screen can be measured in 2 ways: Coordinates or Pixels
* Vulkan works with pixels, swapchain extent must be specified in pixels.
*
* Some High DPI Display's coordinates is not corresponding to pixels.
* Therefore, transformation is needed.
*/
VkExtent2D MySwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	// the braceket is supper important, else max will be precompiled due
	// to max(a, b) macro defined in windows.h, ref:
	// https://stackoverflow.com/questions/27442885/syntax-error-with-stdnumeric-limitsmax
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		this->window->getFrameBufferSize(&width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width,
			capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height,
			capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}