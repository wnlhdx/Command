
#include <vulkan/vulkan.h>
#include <iostream>


int main() {
    // ºÏ≤ÈVulkan∞Ê±æ
    uint32_t apiVersion = VK_API_VERSION_1_0;
    VkInstance instance;
    VkResult result = vkCreateInstance(nullptr, nullptr, &instance);
    if (result == VK_SUCCESS) {
        std::cout << "Vulkan instance created successfully!" << std::endl;
        vkDestroyInstance(instance, nullptr);
    }
    else {
        std::cerr << "Failed to create Vulkan instance." << std::endl;
    }

    return 0;
}