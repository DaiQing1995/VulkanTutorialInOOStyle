#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>

class MyGraphicsPipeline
{
public:
	MyGraphicsPipeline(VkDevice device, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat);
	~MyGraphicsPipeline();

private:
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
	void setupRenderPass(VkFormat swapChainImageFormat);

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	VkPipeline graphicsPipeline;
};

#endif // !GRAPHICS_PIPELINE_H