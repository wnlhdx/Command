#include <jni.h>
#include <string>
#include <vulkan/vulkan.h>

// 定义Vulkan实例和物理设备
VkInstance instance;
VkPhysicalDevice physicalDevice;

// 检查设备是否适合
bool checkDeviceSuitable(VkPhysicalDevice device) {
    // 这里应该检查设备是否支持你需要的特性
    return true;
}

// 创建Vulkan实例
bool createInstance() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // 启用验证层（如果需要）
    // createInfo.enabledLayerCount = 1;
    // createInfo.ppEnabledLayerNames = &validationLayerName;

    return vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS;
}

// 选择物理设备
bool pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (checkDeviceSuitable(device)) {
            physicalDevice = device;
            return true;
        }
    }

    return false;
}

// 从Java层调用的Native方法
extern "C" JNIEXPORT void JNICALL
Java_com_example_vulkan_MainActivity_initVulkan(JNIEnv* env, jobject /* this */) {
    // 尝试创建Vulkan实例
    if (!createInstance()) {
        throw std::runtime_error("failed to create instance!");
    }

    // 尝试选择物理设备
    if (!pickPhysicalDevice()) {
        throw std::runtime_error("failed to pick physical device!");
    }

    // 这里可以继续Vulkan的初始化，例如创建设备，交换链等
}

// 原来的stringFromJNI方法，用于测试
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_vulkan_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
