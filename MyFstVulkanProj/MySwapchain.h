#ifndef MY_SWAPCHAIN_H
#define MY_SWAPCHAIN_H

#include "BasicEntities.h"
#include "MyWindow.h"

class MySwapchain
{
public:
	MySwapchain(const MyWindow* window, const SwapChainSupportDetails* swapchainSD,
		VkSurfaceKHR surface, VkDevice device, uint32_t gfxIdx, uint32_t pstIdx);

	~MySwapchain();

	inline VkFormat getSwapChainImageFmt() { return swapChainImageFormat; }
	inline VkExtent2D getSwapChainExtent() { return swapChainExtent; }

private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapChainImages;	// Saving images in swapchain
	std::vector<VkImageView> swapChainImageViews;

	// Record swapchain image info
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	const MyWindow *window;
	const VkDevice device;
};

#endif // !MY_SWAPCHAIN_H