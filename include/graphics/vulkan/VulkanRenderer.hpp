﻿#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "graphics/Mesh.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanMemoryManager.hpp"
#include "graphics/vulkan/VulkanPipeline.hpp"
#include "graphics/vulkan/VulkanQueues.hpp"
#include "graphics/vulkan/VulkanSwapchain.hpp"

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
  struct VulkanRendererState{
    unsigned int currentFrame = 0;
    unsigned int imageIndex = 0;
    bool acquiredImageStillValid = false;
    bool doNotRender = false;
  } mState;

  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  RenderWindow *mWindow;

  VkInstance mInstance{};
  VkSurfaceKHR mSurface;

  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mDevice{};

  VulkanQueues mQueues;

  VulkanMemoryManager mMemoryManager;

  VulkanSwapchain mSwapchain;
  VulkanPipeline mPipeline{};
  VkRenderPass mRenderPass{};

  VkCommandPool mCommandPool{};
  std::vector<VkCommandBuffer> mCommandBuffers;

  static constexpr unsigned int mMaxFramesInFlight = 2;
  std::array<VkSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores{};
  std::array<VkSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores{};
  std::array<VkFence, mMaxFramesInFlight> mInFlightFences{};
  std::vector<VkFence> mImagesInFlight;

  void createVulkanInstance();
  void selectPhysicalDevice();
  [[nodiscard]] static unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                       VkSurfaceKHR const &vulkanSurface);
  [[nodiscard]] static bool
  physicalDeviceSupportsRequiredExtensions(VkPhysicalDevice const &physicalDevice);
  void createVulkanDevice();
  void createRenderPass();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void handleFrameBufferResize();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  VulkanBuffer createMeshBuffer(Mesh const &mesh);
  void destroyMeshBuffer(VulkanBuffer meshBuffer);

  bool acquireNextFrame();
  void startDraw();
  void drawMesh(Mesh const &mesh, VulkanBuffer const &meshBuffer);
  void endDraw();
  void present();

  void stop();
};
} // namespace flex
