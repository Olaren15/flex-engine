#pragma once

#include <vector>

#include "graphics/Vertex.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"

namespace flex {
struct Mesh {
  explicit Mesh(std::vector<uint32_t> const &indices, std::vector<Vertex> const &vertices);

  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  VulkanBuffer buffer;
  glm::mat4 position{1};
  uint32_t shaderId = UINT32_MAX;

  [[nodiscard]] size_t getIndicesSize() const;
  [[nodiscard]] size_t getVerticesSize() const;
  [[nodiscard]] size_t getRequiredBufferSize() const;
};
} // namespace flex