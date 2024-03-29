#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "Application.h"

class MyGraphicsPipeline
{
public:
	friend class MyFramebuffer;
	MyGraphicsPipeline(VkDevice device, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, Application *app);
	~MyGraphicsPipeline();
	inline VkPipeline getPipeline() { return graphicsPipeline; }
	inline VkRenderPass getRenderPass() { return renderPass; }

private:
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
	void setupRenderPass(VkFormat swapChainImageFormat);

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	VkPipeline graphicsPipeline;

	Application* app;
};

#endif // !GRAPHICS_PIPELINE_H