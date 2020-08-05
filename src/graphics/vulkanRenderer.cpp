#include "graphics/vulkanRenderer.hpp"

namespace flex {
VulkanRenderer::VulkanRenderer(const Window &window) {
  if (window.getRenderAPI() != RenderAPI::Vulkan) {
    throw std::runtime_error("Can't create vulkan renderer if window is not "
                             "initialized with the Vulkan render API");
  }

  createVulkanInstance(window);
}

VulkanRenderer::~VulkanRenderer() { mVulkanInstance.destroy(); }

void VulkanRenderer::createVulkanInstance(const Window &window) {
  vk::ApplicationInfo appInfo(window.getTitle().c_str(),
                              VK_MAKE_VERSION(1, 0, 0), "Flex Engine",
                              VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_0);

  std::vector<const char *> enabledExtensions =
      window.getRequiredVulkanExtensions();

  std::vector<const char *> enabledLayers{};

  if (mEnableValidationLayers) {
    enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabledLayers,
                                            enabledExtensions);

  mVulkanInstance = vk::createInstance(instanceCreateInfo);
}
} // namespace flex