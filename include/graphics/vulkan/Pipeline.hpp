#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

namespace flex {
struct Pipeline {
private:
  [[nodiscard]] VkShaderModule createShaderModule(VkDevice const &device,
                                                  std::filesystem::path const &shaderPath) const;

public:
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
  VkDynamicState dynamicState;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  Pipeline() = default;
  Pipeline(Pipeline const &pipeline) = delete;

  void createPipeline(VkDevice const &device, VkRenderPass const &renderPass);
  void destroy(VkDevice const &device) const;
};
} // namespace flex
