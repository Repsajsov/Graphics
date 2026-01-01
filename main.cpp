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

  // Hoeveel gpu's heb ik.?
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  // Geef me lijst van al die devices
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  if (deviceCount == 0) {
    std::cout << "FOUT: " << "Geen gpu gevonden!" << std::endl;
    return -1;
  }

  VkPhysicalDevice physicalDevice = devices[0];
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  std::cout << "Naam GPU: " << deviceProperties.deviceName << std::endl;

  // -------------------------------------------------------------------

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  uint32_t graphicsQueueFamily = UINT32_MAX;
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsQueueFamily = i;
      std::cout << "Graphics queue gevonden: " << i << std::endl;
      break;
    }
  }

  if (graphicsQueueFamily == UINT32_MAX) {
    std::cout << "FOUT: Geen graphics queue gevonden!" << std::endl;
    return -1;
  }

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
  queueCreateInfo.queueCount = 1;
  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  deviceCreateInfo.enabledExtensionCount = 1;
  const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
  VkDevice logicalDevice;
  result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr,
                          &logicalDevice);

  if (result != VK_SUCCESS) {
    std::cout << "FOUT bij logical device: " << result << std::endl;
    return -1;
  }

  std::cout << "Logical device gemaakt!" << std::endl;

  VkQueue graphicsQueue;
  vkGetDeviceQueue(logicalDevice, graphicsQueueFamily, 0, &graphicsQueue);
  std::cout << "Graphics queue opgehaald!" << std::endl;

  VkSurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
    std::cout << "FOUT bij surface!" << SDL_GetError() << std::endl;
    return -1;
  }

  std::cout << "Surface gemaakt!" << std::endl;

  VkBool32 presentQueue = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamily,
                                       surface, &presentQueue);

  if (!presentQueue) {
    std::cout << "FOUT: Graphics queue niet present!" << std::endl;
    return -1;
  }

  std::cout << "Present support: OK" << std::endl;
  // swapchain capabilities
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &capabilities);

  std::cout << "Min images: " << capabilities.minImageCount << std::endl;
  std::cout << "Max images: " << capabilities.maxImageCount << std::endl;

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       formats.data());

  std::cout << "Beschikbare formaten: " << formatCount << std::endl;

  VkSurfaceFormatKHR surfaceFormat = formats[0];
  std::cout << "Gekozen format: " << surfaceFormat.format << std::endl;

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentModeCount, presentModes.data());

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  std::cout << "Present mode: FIFO (vsync)" << std::endl;

  // Swapchain maken?
  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 &&
      imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }
  swapchainCreateInfo.minImageCount = imageCount;

  // format en kleuren
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

  // resolutie
  VkExtent2D extent;
  if (capabilities.currentExtent.width != UINT32_MAX) {
    extent = capabilities.currentExtent;
  } else {
    int width, height;
    SDL_Vulkan_GetDrawableSize(window, &width, &height);
    extent.width = std::max(capabilities.minImageExtent.width,
                            std::min(capabilities.maxImageExtent.width,
                                     static_cast<uint32_t>(width)));
    extent.height = std::max(capabilities.minImageExtent.height,
                             std::min(capabilities.maxImageExtent.height,
                                      static_cast<uint32_t>(height)));
  }

  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // queue families
  swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = 0;
  swapchainCreateInfo.pQueueFamilyIndices = nullptr;

  // transforms en alpha
  swapchainCreateInfo.preTransform = capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapchain;
  result = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr,
                                &swapchain);
  if (result != VK_SUCCESS) {
    std::cout << "FOUT bij swapchain!" << std::endl;
    return -1;
  }

  std::cout << "Swapchain gemaakt met " << imageCount << " images!"
            << std::endl;

  uint32_t swapchainImageCount;
  vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount,
                          nullptr);
  std::vector<VkImage> swapchainImages(swapchainImageCount);
  vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount,
                          swapchainImages.data());

  std::cout << "Swapchain images opgehaald: " << swapchainImageCount
            << std::endl;

  // bool running = true;
  // SDL_Event event;

  // while (running) {
  //   while (SDL_PollEvent(&event)) {
  //     if (event.type == SDL_QUIT)
  //       running = false;
  //     if (event.type == SDL_KEYDOWN && event.key.keysym.sym ==
  //     SDLK_ESCAPE) {
  //       running = false;
  //     }
  //   }

  //   SDL_Delay(16);
  // }

  vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyDevice(logicalDevice, nullptr);
  vkDestroyInstance(instance, nullptr);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}