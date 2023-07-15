#include <stdexcept>
#include <string>
#include "MyBufferHelper.h"

/**
* [Memory Heap] Memory needs to be allocated from different heap that GPU
* owns, the memory is marked by propertyFlags, using these
* flags to find a suitable memory resources.
* 
* [Memory Type] Memory got different type like dedicated VRAM and swap space.
* Use a type Filter to control it.
* 
* @ret: the memory index of the physical device
*/
uint32_t BufferHelper::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_phyDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find a suitable memory type!");
}

void BufferHelper::copyCPUData2Memory(VkBuffer buffer, const void* src, size_t size) {
	void* pMap;
	if (createdBufferMaps.find(buffer) != createdBufferMaps.end()) {
		// dev, memory, offset of memory, flags, data ptr
		vkMapMemory(m_device, createdBufferMaps[buffer], 0, size, 0, &pMap);
		memcpy(pMap, src, size);
		/**
		* The memcpy may not success for some type of memory immediately except
		* the following 2 ways:
		*
		* - Use a memory heap that is host coherent, indicated with 
		* VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
		* 
		* - Call vkFlushMappedMemoryRanges after writing to the mapped 
		* memory, and call vkInvalidateMappedMemoryRanges before reading
		* from the mapped memory.
		* But this way relies on the next vkQueueSubmit call.
		*/
		vkUnmapMemory(m_device, createdBufferMaps[buffer]);
	}
	else {
		// TODO: Consider to create lazy memory creation
	}
}

void BufferHelper::createBufferWithMemory(VkBufferCreateInfo& bufferInfo, VkBuffer& buffer) {
	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	// TODO: Currently the memory type is hard-coded, need reconstuct.
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkDeviceMemory host_mappable_memory;
	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &host_mappable_memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Vertex buffer Memory!");
	}

	// The image can have different required size with the memory, 
	// here the forth parameter is offset within the memory.
	vkBindBufferMemory(m_device, buffer, host_mappable_memory, 0);
	createdBufferMaps[buffer] = host_mappable_memory;
}

BufferHelper::~BufferHelper() {
	for (auto it = createdBufferMaps.begin(); it != createdBufferMaps.end(); ++it) {
		vkDestroyBuffer(m_device, it->first, nullptr);
		vkFreeMemory(m_device, it->second, nullptr);
	}
}