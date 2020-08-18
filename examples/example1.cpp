#include <iostream>

#include <flexEngine.hpp>

#ifdef _WIN32
#include <Windows.h>
// ReSharper disable once CppInconsistentNaming
int wWinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
             [[maybe_unused]] _In_ LPWSTR lpCmdLine, [[maybe_unused]] _In_ int nShowCmd) {
#else
int main() {
#endif // _WIN32

  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::Window window{"window", width, height, fullscreen};
  flex::VulkanRenderer vulkanRenderer{window};

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Space")) {
      std::cout << "pew pew !" << std::endl;
    }
  }

  return 0;
}