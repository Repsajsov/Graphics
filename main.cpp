#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

int main() {
  SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("Wayland Test", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, 800, 600,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

  if (!window) {
    std::cout << "Window fail: " << SDL_GetError() << "\n";
    return -1;
  }

  // Vk dingesss??...
  VkInstance instance;

  // Basale informatie die gpu driver wil weten.
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Mijn applicatie ofzo??";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Geen engine...";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // Extra dingen?
  uint32_t extensionCount;
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
  std::vector<const char *> extensions(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

  // Vertel me als ik fouten maak dinges?
  const char *layers[] = {"VK_LAYER_KHRONOS_validation"};

  // Alles bij elkaar dus de appinfo, de extensiedinges, en iets met errors
  // laten zien ofzo?
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = extensionCount;
  createInfo.ppEnabledExtensionNames = extensions.data();
  createInfo.enabledLayerCount = 1;
  createInfo.ppEnabledLayerNames = layers;

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

  if (result != VK_SUCCESS) {
    std::cout << "FOUT: " << result << std::endl;
  }

  // SDL_Renderer *renderer =
  //     SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // if (!renderer) {
  //   std::cout << "Renderer fail: " << SDL_GetError() << "\n";
  //   SDL_DestroyWindow(window);
  //   SDL_Quit();
  //   return -1;
  // }

  // bool running = true;
  // SDL_Event event;

  // while (running) {
  //   while (SDL_PollEvent(&event)) {
  //     if (event.type == SDL_QUIT)
  //       running = false;
  //     if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
  //       running = false;
  //     }
  //   }

  //   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  //   SDL_RenderClear(renderer);

  //   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  //   SDL_Rect rect = {350, 250, 100, 100};
  //   SDL_RenderFillRect(renderer, &rect);

  //   SDL_RenderPresent(renderer);
  //   SDL_Delay(16);
  // }

  // SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}