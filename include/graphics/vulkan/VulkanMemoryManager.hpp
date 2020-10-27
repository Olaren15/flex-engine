#pragma once

#include "vk_mem_alloc.h"

#include "graphics/Mesh.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanImage.hpp"
#include "graphics/vulkan/VulkanQueues.hpp"

namespace flex {
struct VulkanMemoryManager {
private:
  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mDevice{};

  VmaAllocator mAllocator{};
  VkQueue mTransferQueue{};
  VulkanQueueFamilyIndices mQueueFamilyIndices;

  VkCommandPool mTransferCommandPool{};
  VkCommandBuffer mTransferCommandBuffer{};

  VkBufferCreateInfo buildTransferBufferCreateInfo(VkDeviceSize const &bufferSize);

  void beginTransferCommandBuffer() const;
  void endTransferCommandBuffer() const;

  void createStagingBuffer(VulkanBuffer &stagingBuffer, VkDeviceSize const &bufferSize);

  void copyBufferToBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer,
                          VkDeviceSize const &bufferSize, VkDeviceSize srcOffset,
                          VkDeviceSize dstOffset) const;

  void transferBufferOwnership(VkBuffer const &buffer, uint32_t srcQueueFamilyIndex,
                               uint32_t dstQueueFamilyIndex) const;

public:
  VulkanMemoryManager() = default;
  VulkanMemoryManager(VulkanMemoryManager const &) = delete;

  void initialize(VkInstance const &instance, VkPhysicalDevice const &physicalDevice,
                  VkDevice const &device, VulkanQueues const &queues);
  void destroy() const;
  void destroyBuffer(VulkanBuffer const &buffer) const;

  VulkanBuffer buildMeshBuffer(Mesh const &mesh);
  void updateMeshBuffer(VulkanBuffer meshBuffer, Mesh const &mesh);

  void copyDataToBuffer(void const *srcData, VulkanBuffer &dstBuffer, VkDeviceSize const &dataSize,
                        VkDeviceSize const &srcOffset, VkDeviceSize const &dstOffset) const;

  VulkanImage createImage(VkExtent2D const &imageExtent, VkFormat const &format,
                          VkImageTiling const &tiling, VkImageUsageFlags const &usage,
                          VkImageAspectFlags const &imageAspect);
  void destroyImage(VulkanImage &image);

  VulkanImage createDepthBufferImage(VkExtent2D const &swapchainExtent);
};
} // namespace flex