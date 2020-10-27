﻿#include "graphics/vulkan/VulkanSwapchain.hpp"

#include <algorithm>
#include <set>
#include <stdexcept>

#include "graphics/vulkan/VulkanHelpers.hpp"
#include "graphics/vulkan/VulkanMemoryManager.hpp"

namespace flex {
VkSurfaceFormatKHR VulkanSwapchain::chooseSwapchainSurfaceFormat(
    std::vector<VkSurfaceFormatKHR> const &availableFormats) {
  if (availableFormats.empty()) {
    throw std::runtime_error("Cannot choose a format from an empty array");
  }

  for (VkSurfaceFormatKHR const &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats.front();
}

VkPresentModeKHR VulkanSwapchain::chooseSwapchainPresentMode(
    std::vector<VkPresentModeKHR> const &availablePresentModes) {

  const std::set<VkPresentModeKHR> rankedModes{VK_PRESENT_MODE_MAILBOX_KHR,
                                               VK_PRESENT_MODE_IMMEDIATE_KHR};

  for (VkPresentModeKHR const &presentMode : rankedModes) {
    auto mode = std::find(availablePresentModes.begin(), availablePresentModes.end(), presentMode);
    if (mode != availablePresentModes.end()) {
      return *mode;
    }
  }

  // only guaranteed mode
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::chooseSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                  RenderWindow const &window) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    // Cannot decide on the extent size
    return capabilities.currentExtent;
  }

  VkExtent2D const windowDrawableSize = window.getDrawableVulkanSurfaceSize();

  uint32_t const actualWidth =
      std::clamp(windowDrawableSize.width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);
  uint32_t const actualHeight =
      std::clamp(windowDrawableSize.height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  return VkExtent2D{actualWidth, actualHeight};
}

void VulkanSwapchain::retrieveSwapchainImages(VkDevice const &device) {
  uint32_t swapchainImageCount;
  validateVkResult(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr));
  images.resize(swapchainImageCount);
  validateVkResult(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, images.data()));
}

void VulkanSwapchain::createImageViews(VkDevice const &device) {
  imageViews.resize(images.size(), {});

  VkImageSubresourceRange subresourceRange;
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  VkComponentMapping componentMapping;
  componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  for (size_t i = 0; i < images.size(); i++) {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = images[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components = componentMapping;
    imageViewCreateInfo.subresourceRange = subresourceRange;

    validateVkResult(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]));
  }
}

void VulkanSwapchain::createSwapchain(VkPhysicalDevice const &physicalDevice,
                                      VkDevice const &device, RenderWindow const &window,
                                      VkSurfaceKHR const &surface,
                                      VulkanQueueFamilyIndices const &queueFamilyIndices) {
  swapchainSupportDetails = VulkanSwapchainSupportDetails{physicalDevice, surface};

  VkSurfaceFormatKHR const surfaceFormat =
      chooseSwapchainSurfaceFormat(swapchainSupportDetails.formats);

  format = surfaceFormat.format;

  VkPresentModeKHR const presentMode =
      chooseSwapchainPresentMode(swapchainSupportDetails.presentModes);

  extent = chooseSwapchainExtent(swapchainSupportDetails.capabilities, window);

  uint32_t const minimumImageCount =
      swapchainSupportDetails.capabilities.maxImageCount > 0
          ? std::min(swapchainSupportDetails.capabilities.minImageCount + 1,
                     swapchainSupportDetails.capabilities.maxImageCount)
          : swapchainSupportDetails.capabilities.minImageCount + 1;

  std::set<uint32_t> uniqueQueueFamilyIndices{queueFamilyIndices.graphics.value(),
                                              queueFamilyIndices.present.value()};
  std::vector<uint32_t> queueFamiliesIndices{uniqueQueueFamilyIndices.begin(),
                                             uniqueQueueFamilyIndices.end()};

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = minimumImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (queueFamiliesIndices.size() > 1)
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  else
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamiliesIndices.size());
  swapchainCreateInfo.pQueueFamilyIndices = queueFamiliesIndices.data();
  swapchainCreateInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = swapchain;

  validateVkResult(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

  retrieveSwapchainImages(device);
  createImageViews(device);
}

void VulkanSwapchain::createFrameBuffers(VkDevice const &device, VkRenderPass renderPass,
                                         VulkanImage const &depthImage) {
  framebuffers.resize(imageViews.size(), {});

  for (size_t i = 0; i < imageViews.size(); i++) {
    std::array<VkImageView, 2> attachments{imageViews[i], depthImage.imageView};

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferCreateInfo.pAttachments = attachments.data();
    framebufferCreateInfo.width = extent.width;
    framebufferCreateInfo.height = extent.height;
    framebufferCreateInfo.layers = 1;

    validateVkResult(
        vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]));
  }
}

void VulkanSwapchain::handleFrameBufferResize(VkPhysicalDevice const &physicalDevice,
                                              VkDevice const &device, RenderWindow const &window,
                                              VkSurfaceKHR const &surface,
                                              VulkanQueueFamilyIndices const &queueFamilyIndices,
                                              VkRenderPass const &renderPass,
                                              VulkanImage &depthBufferImage) {

  for (VkFramebuffer &framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  for (VkImageView &imageView : imageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }

  // ReSharper disable once CppLocalVariableMayBeConst
  VkSwapchainKHR oldSwapchain = swapchain;
  createSwapchain(physicalDevice, device, window, surface, queueFamilyIndices);

  VulkanMemoryManager &memoryManager = depthBufferImage.memoryManager;
  memoryManager.destroyImage(depthBufferImage);
  depthBufferImage = memoryManager.createDepthBufferImage(extent);

  createFrameBuffers(device, renderPass, depthBufferImage);

  vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
}

void VulkanSwapchain::destroy(VkDevice const &device) const {
  for (VkFramebuffer const &framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  for (VkImageView const &imageView : imageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(device, swapchain, nullptr);
};

} // namespace flex
