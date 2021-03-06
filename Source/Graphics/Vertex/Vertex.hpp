#pragma once

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace cbl::gfx {
struct Vertex {
  glm::vec3 position;
  glm::vec3 uvw;

  static VkVertexInputBindingDescription getVulkanBindingDescription();
  static std::array<VkVertexInputAttributeDescription, 2> getVulkanAttributeDescriptions();
};
} // namespace cbl::gfx