#ifndef MY_BUFFER_HELPER
#define MY_BUFFER_HELPER

#include <vulkan/vulkan.hpp>
#include <unordered_map>

/**
* The helper class for buffer creation & destroy.
* 
* It also aims to become a buffer pool for managing and the VkBuffer.
*/
class BufferHelper {
public:
	BufferHelper(VkDevice device, VkPhysicalDevice phyDevice) :m_device(device), m_phyDevice(phyDevice){};
	~BufferHelper();
	void createBufferWithMemory(VkBufferCreateInfo &bufferInfo, VkBuffer &buffer);

	void copyCPUData2Memory(VkBuffer mem, const void* src, size_t size);

private:

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkDevice m_device;
	VkPhysicalDevice m_phyDevice;
	std::unordered_map<VkBuffer, VkDeviceMemory> createdBufferMaps;
};

#endif