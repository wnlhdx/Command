#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <fstream>

using namespace std;
void processInput(GLFWwindow* window);
//[2]验证层Debug时开启
#ifdef NDEBUG    //不调试
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
int errorCounter = 0;
const std::string getAssetPath()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    return "";
#elif defined(VK_EXAMPLE_DATA_DIR)
    return VK_EXAMPLE_DATA_DIR;
#else
    return "./../data/";
#endif
}
//[2]所有有用的标准验证都捆绑到SDK的一个层中，称为VK_LAYER_KHRONOS_validation层。
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

//[5]表示满足需求的队列族
struct QueueFamilyIndices {
    //我们经常遇到这样的情况:我们可能返回/传递/使用某种类型的对象。也就是说，我们可以有某个类型的值，也可以没有任何值。因此，我们需要一种方法来模拟类似指针的语义，在指针中，我们可以使用nullptr来表示没有值。处理这个问题的方法是定义一个特定类型的对象，并用一个额外的布尔成员 / 标志来表示值是否存在。std::optional<>以一种类型安全的方式提供了这样的对象。std::optional对象只是包含对象的内部内存加上一个布尔标志。因此，大小通常比包含的对象大一个字节

    std::optional<uint32_t> graphicsFamily;        //该字段表示满足需求的队列族
    std::optional<uint32_t> presentFamily;        //支持表现的队列族索引
    //[8]为了方便起见，我们还将向结构本身添加一个泛型检查
    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

//[5]
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

//[5]设备扩展列表，检测VK_KHR_swapchain
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

//[7]
const int WIDTH = 800;
//[7]
const int HEIGHT = 600;


//[10]ate: Start reading at the end of the file
//[10]binary: Read the file as binary file (avoid text transformations)
static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    //[10]ate 的优势是，可以获取文件的大小
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    //[10]指针跳到头
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}


//[14]
const int MAX_FRAMES_IN_FLIGHT = 2;        //可以同时并行处理的帧数


class HelloTriangle {

public:
    void run() {
        //[1]
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
public:
    //[1]
    GLFWwindow* window;
    //[2]
    VkInstance instance;    //创建instance
    //[3]
    VkSurfaceKHR surface;    //窗口表面
    //[4]
    VkDebugUtilsMessengerEXT debugMessenger;
    //[5]当vkinInstance被销毁时，该对象将被隐式销毁，因此不需要在cleanup函数中执行销毁。
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    //[6]
    VkDevice device;    //逻辑设备对象作为类成员
    //[6]
    VkQueue graphicsQueue;
    //[6]
    VkQueue presentQueue;    //呈现队列
    //[7]
    VkSwapchainKHR swapChain;    //交换链
    //[7]
    std::vector<VkImage> swapChainImages;    //交换链的图像句柄, 在交换链清除时自动被清除
    //[7]
    VkFormat swapChainImageFormat;        //交换链图像格式
    //[7]
    VkExtent2D swapChainExtent;            //交换链图像范围
    //[8]
    std::vector<VkImageView> swapChainImageViews;    //存储图像视图
    //[9]
    VkRenderPass renderPass;
    //[10]
    VkPipelineLayout pipelineLayout;
    //[10]
    VkPipeline graphicsPipeline;
    //[11]
    std::vector<VkFramebuffer> swapChainFramebuffers;        //存储所有帧缓冲对象
    //[12]
    VkCommandPool commandPool;    //创建指令池对象， //指令池对象,管理指令缓冲对象使用的内存，并负责指令缓冲对象的分配

    //[13] Command buffers will be automatically freed when their command pool is destroyed
    std::vector<VkCommandBuffer> commandBuffers;    //指令池对象,管理指令缓冲对象使用的内存，并负责指令缓冲对象的分配

    //[14]为每一帧创建属于它们自己的信号量，信号量发出图像已经被获取，可以开始渲染的信号
    std::vector<VkSemaphore> imageAvailableSemaphores;
    //[14]信号量发出渲染已经有结果，可以开始呈现的信号
    std::vector<VkSemaphore> renderFinishedSemaphores;
    //[14] 需要使用栅栏(fence) 来进行 CPU 和 GPU 之间的同步， 来防止有超过 MAX_FRAMES_IN_FLIGHT帧的指令同时被提交执行。
    //    栅栏(fence) 和信号量(semaphore) 类似，可以用来发出信号和等待信号
    std::vector<VkFence> inFlightFences;        //每一帧创建一个VkFence 栅栏对象
    //[14]-[15]是否需要释放？
    std::vector<VkFence> imagesInFlight;

    //[15]
    size_t currentFrame = 0;          //追踪当前渲染的是哪一帧

    //[16]
    bool framebufferResized = false;    //标记窗口大小是否发生改变

    //[1]
    void initWindow() {
        glfwInit();        //初始化glfw库
        //因为glfw最初是为创建OpenGL上下文而设计的，所以我们要告诉它不要通过后续调用创建OpenGL上下文：GLFW_NO_API
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);        //窗口大小设置为不可变
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Study Yue Qingxuan", nullptr, nullptr);    //创建窗口
        //[16-1]储存当前对象指针
        glfwSetWindowUserPointer(window, this);
        //[1] 检测窗体实际大小，我们可以使用 GLFW 框架中的 glfwSetFramebufferSizeCallback 函数来设置回调：
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
    void initVulkan() {
        //[2]
        createInstance();
        //[3]
        createSurface();
        //[4]
        setupDebugMessenger();
        //[5]
        pickPhysicalDevice();
        //[6]
        createLogicalDevice();
        //[7]
        createSwapChain();
        //[8]
        createImageViews();
        //[9]
        createRenderPass();
        //[10]
        createGraphicsPipeline();
        //[11]
        createFramebuffers();
        //[12]
        createCommandPool();
        //[13]
        createCommandBuffers();
        //[14]
        createSemaphores();
    }
    void mainLoop() {
        //[1]
        while (!glfwWindowShouldClose(window))
        {
            processInput(window);
            glfwPollEvents();
            //[15]
            drawFrame();
        }
        //[16]可以用作执行同步的基本的方法.
        vkDeviceWaitIdle(device);
    }

    //VkInstance 应该在应用程序结束前进行清除操作
    void cleanup()
    {
        //[17]
        cleanupSwapChain();//销毁交换链对象，在逻辑设备被清除前调用

        //[14]        //清除为每一帧创建的信号量和VkFence
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {//所有它所同步的指令执行结束后，对它进行清除
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
        //17将这些释放资源的函数移动到cleanupSwapChain中
        //[12]
        //vkDestroyCommandPool(device, commandPool, nullptr);    //清除创建的指令池对象
        //[11]
        //for (auto framebuffer : swapChainFramebuffers) {
        //    vkDestroyFramebuffer(device, framebuffer, nullptr);
        //}
        //[10]
        //vkDestroyPipeline(device, graphicsPipeline, nullptr);
        //[10]
        //vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        //[9]
        //vkDestroyRenderPass(device, renderPass, nullptr);
        //[8]
        //for (auto imageView : swapChainImageViews) {
        //    vkDestroyImageView(device, imageView, nullptr);
        //}

        //[7]
        //vkDestroySwapchainKHR(device, swapChain, nullptr);
        //[6]逻辑设备并不直接与 Vulkan 实例交互，所以创建逻辑设备时不需要使用 Vulkan 实例作为参数
        vkDestroyDevice(device, nullptr);    //清除逻辑设备对象
        //[4]
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            cout << "000 cleanup enableValidationLayers=" << enableValidationLayers << endl;
        }
        else
        {
            cout << "111 cleanup enableValidationLayers=" << enableValidationLayers << endl;
        }
        //[3]
        vkDestroySurfaceKHR(instance, surface, nullptr);//销毁窗口表面对象，表面对象的清除需要在 Vulkan 实例被清除之前完成
        //[2]
        vkDestroyInstance(instance, nullptr);    //销毁Vulkan实例
        //[1]
        glfwDestroyWindow(window);    //销毁窗口
        glfwTerminate();    //结束glfw
    }


    /// <summary>
    ///[2]创建实例，实例描述了应用程序和可以支持的API扩展，比如VkApplicationInfo里设置应用程序名称、引擎名称
    /// VkInstanceCreateInfo：创建VkInstance，设置全局的扩展和验证层，第一个扩展关于窗口所需要的扩展，有了实例才可以选择物理设备
    /// </summary>
    void createInstance() {
        //[2]这里检测是否启用验证层，这里enableValidationLayers=false，checkValidationLayerSupport()=true
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        else
        {
            cout << "createInstance enableValidationLayers=" << enableValidationLayers << ", !checkValidationLayerSupport()= " << !checkValidationLayerSupport() << std::endl<<"函数返回" << std::endl;
        }
        // 创建一个实例首先必须填写一个包含有关我们应用程序的信息的结构: VkApplicationInfo
        // 这些数据在技术上不是必须的，而是可选的，但它可为驱动程序提供一些有用的信息，以便针对我们的特定应用进行优化
        //[2]well-known graphics engine 
        VkApplicationInfo appInfo = {};    //创建用于描述应用程序信息的结构体
        //[2]结构体必须指明类型，pNext指向拓展信息
        // Vulkan中的许多结构要求在sType成员中明确指定类型，这也是具有pNext成员的许多结构中的一个，该成员可以在将来指向扩展信息。
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;    //该行我自己添加的
        appInfo.pApplicationName = "Hello Triangle";
        //appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
        appInfo.pEngineName = "No Engine";
        //appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        //appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        //[2]Vulkan驱动程序使用哪些全局扩展和验证，后续后详细说明 
        // Vulkan中的很多信息都是通过结构而不是函数参数传递的，我们必须再填充一个结构体 VkInstanceCreateInfo 来为创建实例提供足够的信息。VkInstanceCreateInfo结构是必须指明的，它告诉Vulkan驱动程序我们想要使用哪些全局扩展和验证层。
        VkInstanceCreateInfo createInfo = {};
        //几乎每个Vulkan结构中用于向API传递参数的第一个成员是sType字段，它告诉Vulkan这是什么类型的结构。sType字段均有一个约定俗成的套路,可以对照类名给sType赋值
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;    //用于放置用于描述应用程序信息的结构体(的地址)


        //[2]指定全局扩展
        // Vulkan是一个与平台无关的API，这意味着需要一个与窗口系统接口的扩展。
        // GLFW有一个方便的内置函数，它返回它需要做的扩展，我们可以传递给结构体：VkInstanceCreateInfo
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // 此函数返回GLFW所需的Vulkan实例扩展名的数组，以便为GLFW窗口创建Vulkan surface。
       // 如果成功，列表将始终包含`VK_KHR_surface`，因此如果您不需要任何其他扩展，则可以将此列表直接传递给`VkInstanceCreateInfo`结构。
       // 如果机器上没有Vulkan，则此函数返回“NULL”并生成 GLFW_API_UNAVAILABLE错误。

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        //[2]the global validation layers to enable
        createInfo.enabledLayerCount = 0; //后续有说明
        //[2]验证层信息
        //[2]如果检查成功，那么vkCreateInstance不会返回VK_ERROR_LAYER_NOT_PRESENT错误
        if (enableValidationLayers) {
            // 结构体的最后两个成员确定要启用的全局验证层
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }
        //[2]GLFW
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        //[2]重用
        //[2]通过该方式创建一个额外的调试信息，它将在vkCreateInstance和vkDestroyInstance期间自动创建和销毁
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
        //[2]or
        /*if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }*/
        //[2]    VK_SUCCESS or Error Code
        //[2]VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        //[2]or
        //[2]创建实例    
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance，创建Vulkan实例失败!");
            /*
            * //[2]验证层说明，Vulkan每次调用都会进行相应的验证，通过返回值判定函数是否执行成功
            VkResult vkCreateInstance(
                const VkInstanceCreateInfo * pCreateInfo,
                const VkAllocationCallbacks * pAllocator,
                VkInstance * instance) {

                    if (pCreateInfo == nullptr || instance == nullptr) {
                        log("Null pointer passed to required parameter!");
                        return VK_ERROR_INITIALIZATION_FAILED;

                    }
                return real_vkCreateInstance(pCreateInfo, pAllocator, instance);
            }
            */

        }


        //[2]the number of extensions
        //[2]支持扩展的数量
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        //[2]an array of VkExtensionProperties to store details of the extensions.
        //[2]an array to hold the extension details
        //[2]支持的扩展详细信息，返回支持的扩展列表
        std::vector<VkExtensionProperties> extensionsProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProperties.data());

        //[2]query the extension details
        //[2]Each VkExtensionProperties struct contains the name and version of an extension.
        //[2]查询扩展的详细信息
        std::cout << "createInstance：返回可支持的扩展列表，available extensions:" << std::endl;
        std::cout<<"当前文件名为："<<__FILE__<<"，行号为："<<__LINE__<<std::endl;
        int i = 1;
        for (const auto& extension : extensionsProperties) {
            std::cout << i << "," << "\t" << extension.extensionName << std::endl;
            i++;
        }
        std::cout << "当前文件名为：" << __FILE__ << "，行号为：" << __LINE__ << std::endl;
    }

    //[2]list all of the available layers
    //[2]列出所有验证层的信息，请求所有可用的校验层
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        //第一次调用，确定层的数量
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);        //调整数组大小
        //第二次调用，传入数组
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        //[2]查询[vulkan实例]级的层，是否存在验证层信息 layerName = VK_LAYER_KHRONOS_validation
        for (const char* layerName : validationLayers) {
            bool layerFound = false;
            int i = 1;
            for (const auto& layerProperties : availableLayers) {
                std::cout << std::endl << "i=" << i << "，layerName=" << layerName << ", layerProperties.layerName=" << layerProperties.layerName;
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    std::cout << std::endl << "layerName=" << layerName << ",layerProperties.layerName=" << layerProperties.layerName << "，这里找到layer，layerFound返回true" << std::endl;
                    //break;
                }
                i++;
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    //[2]we have to set up a debug messenger with a callback using the VK_EXT_debug_utils extension.
    //[2]我们必须使用VK_EXT_debug_utils扩展，设置一个带有回调的debug messenger，设置回调函数来接受调试信息。
    //本函数的功能：是否根据启用验证层，返回所需的扩展名列表
    std::vector<const char*> getRequiredExtensions() {
        //[5]指定GLFW扩展，但是debug messenger 扩展是有条件添加的
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::cout << "* glfwExtensions=" << *glfwExtensions << std::endl;
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {//启用验证层
            //[2]在这里使用VK_EXT_DEBUG_UTILS_EXTENSION_NAME宏，它等于字符串“VK_EXT_debug_utils”。
            //[2]使用此宏可以避免输入错误，push_back会负责将一个值当成vector对象的尾元素“压到（push）”vector对象的“尾端(back)”
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        else
        {
            std::cout << "系统没有启用校验层，enableValidationLayers=" << enableValidationLayers << std::endl;
        }
        std::cout<< "extensions.size()="<< extensions.size() <<std::endl;
        return extensions;
    }

    //[2]仔细阅读扩展文档，就会发现有一种方法可以专门为这两个函数调用创建单独的 debug utils messenger.
    //[2]它要求您只需在VkInstanceCreateInfo的pNext扩展字段中
    //[2]传递一个指向VkDebugUtilsMessengerCreateInfoEXT结构的指针。
    //[2]首先将messenger创建信息的填充提取到单独的函数中：
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |    //诊断信息
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |    //警告信息
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;        //不合法和可能造成崩溃的操作信息
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    //[2]Add a new static member function called debugCallback with 
    //[2]    the PFN_vkDebugUtilsMessengerCallbackEXT prototype.
    //[2]使用PFN_vkDebugUtilsMessengerCallbackEXT属性添加一个静态函数
    //[2]The VKAPI_ATTR and VKAPI_CALL ensure that the function has the
    //[2]    right signature for Vulkan to call it.
    //[2]使用VKAPI_ATTR和VKAPI_CALL 确保函数具有正确的签名，以便Vulkan调用它
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 诊断信息
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 信息性消息，如资源的创建
        //[2]关于行为的消息,其不一定是错误，但很可能是应用程序中的BUG
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        //[2]关于无效且可能导致崩溃的行为的消息
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
        //[2]可以使用比较操作来检查消息是否与某个严重性级别相等或更差，例如：
        //[2]if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        //[2]    // Message is important enough to show
        //[2]}        
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,    //指明了消息的严重程度
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 发生了一些与规范或性能无关的事件
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 发生了违反规范或一个可能显示的错误
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT 非最优的方式使用Vulkan，进行了可能影响Vulkan性能的行为
        VkDebugUtilsMessageTypeFlagsEXT messageType,        //指明了消息消息的类型


        //[2]消息本身的详细信息, 包括其重要成员：
        //[2]pMessage 以null结尾的调试消息字符串
        //[2]pObjects 与消息相关的Vulkan对象句柄数组
        //[2]objectCount 数组中的对象个数
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,    //该结构体包含了消息更多的细节内容
        //最后一个参数pUserData 包含在回调设置期间指定的指针，并允许您将自己的数据传递给它
        void* pUserData) {
        std::cerr <<"errorCounter=" <<errorCounter << "，validation layer: " << pCallbackData->pMessage << std::endl;
        errorCounter++;
        //通常只在测试校验层本身时会返回true，其余情况下均返回VK_FALSE；
        return VK_FALSE;

    }

    //[2]--------------------------------------------------------------------------------------------------------
    //[3]--------------------------------------------------------------------------------------------------------

    /// <summary>
    /// 使用glfw创建窗口表面
    /// </summary>
    void createSurface() {
        //Windows的创建方法
            //VkWin32SurfaceCreateInfoKHR createInfo = {};
            //createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            //createInfo.hwnd = glfwGetWin32Window(window);
            //
            ////createInfo.hwnd = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);;
            //createInfo.hinstance = GetModuleHandle(nullptr);
            //if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to create window surface!");
            //}
            //Linux的创建方法与上面类似 vkCreateXcbSurfaceKHR
            //使用GLFW创建Window surface，参数依次是 VkInstance 对象，GLFW 窗口指针，自定义内存分配器，存储返回的 VkSurfaceKHR 对象的内存地址        
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        else
        {
            std::cout << "succeed to create window surface" << std::endl;
        }
    }
    //[3]--------------------------------------------------------------------------------
    //[4]--------------------------------------------------------------------------------
    void setupDebugMessenger()
    {
        if (!enableValidationLayers) return;    //该程序从这里直接退出来

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        populateDebugMessengerCreateInfo(createInfo);
        //[4] messenger 创建信息的填充提取到单独的函数中
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            std::cout << "Failed to setup debug messenger" << std::endl;
            //throw std::runtime_error("failed to set up debug messenger!");            
        }
        else
        {
            std::cout << "Succeed to setup debug messenger" << std::endl;
        }
        //[4]or
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        //[4]指定希望调用回调严重性类型
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | //诊断消息
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |        //有关行为的消息不一定是错误，但很可能是应用程序中的错误
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;    //有关无效行为的消息，可能导致崩溃
        //[4]过滤回调通知的消息类型，这里启用了所有类型
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |    //发生了一些与规范或性能无关的事件
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                    //发生了违反规范或一些可能显示的错误，出现了违反规范的情况或发生了一个可能的错误
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;                    //非最优的方式使用Vulkan，进行了可能影响Vulkan性能的行为

        //[4]指定指向回调函数的指针
        createInfo.pfnUserCallback = debugCallback;
        //[4]返回的回调函数
        createInfo.pUserData = nullptr;

    }

    //[4]创建代理函数
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {//[4]如果无法加载，函数将返回nullptr。
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    //[4]销毁CreateDebugUtilsMessengerEXT
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const
        VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
    //[4]--------------------------------------------------------------------------------------------------------

    //--------------[5]-------------

    /// <summary>
    /// pickPhysicalDevice函数最终使我们选择的图形显卡存储在类成员VkPhysicalDevice句柄中。每个GPU设备有一个VkPhysicalDevice类型的句柄。通过GPU设备的句柄，我们可以查询GPU设备的名称，属性，功能等。当VkInstance销毁时，这个对象将会被隐式销毁，所以我们并不需要在cleanup函数中做任何操作。
    /// </summary>
    void pickPhysicalDevice() {
        //[5]查询GPU数量
        uint32_t deviceCount = 0;
        //第一次用vkEnumeratePhysicalDevices函数找到系统中安装的Vulkan兼容设备
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);    //获取图形卡列表，deviceCount为out类型的，存储显卡的数量
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        else
        {
            std::cout << "找到支持Vulkan的显卡（GPU）数量：" << deviceCount << std::endl;
        }
        //[5]获取驱动信息，分配数组存储所有的VkPhysicalDevice句柄
        std::vector<VkPhysicalDevice> devices(deviceCount);
        //第二次用vkEnumeratePhysicalDevices函数把数组devices.data()传入pPhysicalDevices,便可获得支持的[物理设备]的列表
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        //[5]选择适合该程序的GPU
        int i = 0;
        for (const auto& device : devices)
        {  //对显卡进行评估，以确保设备可以在我们创建的表面上显示图像
            std::cout<<"devices.size()"<< devices.size()<<std::endl;
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                std::cout << "i=" << i << ", True,  devices.data()=" << devices[i] << std::endl;
                break;    //如果找到合适的显卡，即退出来
            }
            i++;
        }
        if (physicalDevice == VK_NULL_HANDLE) { //这里的判断条件改用devices.size()==0也是可以的
            //未曾赋值，直接退出
            throw std::runtime_error("failed to find a suitable GPU!");
        }
        //or
        //[5]使用有序Map,通过分数自动对显卡排序，multimap保存的有序的键/值对，但它可以保存重复的元素
        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : devices) {

            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
            std::cout << "本显卡得分：score=" << score << std::endl;
        }

        //[5]Check if the best candidate is suitable at all，检查最佳候选的（物理设备）是否合适
        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        }
        else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }


    //---------------[5]---------------    
    /// <summary>
    /// 评估显卡（GPU）设备是否适合该程序的要求
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    bool isDeviceSuitable(VkPhysicalDevice device) {
        //[5]查询显卡属性，包括：名称，支持Vulkan的版本号
        VkPhysicalDeviceProperties deviceProperties;
        //第一个参数device为输入参数，传入物理设备，第二个参数deviceProperties为输出参数，返回其属性
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "在这里面： isDeviceSuitable， deviceProperties.deviceName=" << deviceProperties.deviceName << "，deviceProperties.limits.maxComputeSharedMemorySize=" << deviceProperties.limits.maxComputeSharedMemorySize << std::endl;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            std::cout << "本显卡为独立显卡" << std::endl;
        else
        {
            std::cout << "本显卡为集成显卡" << std::endl;
        }
        //[5]扩展支持
        bool extensionsSupported = checkDeviceExtensionSupport(device);
        
        //[5]swap chain support
        //检测交换链的能力是否满足需求,我们只能在验证交换链扩展可用后查询交换链的细节信息
        //我们只需要交换链至少支持一种图像格式和一种支持我们的窗口表面的呈现模式
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            std::cout << "isDeviceSuitable  extensionsSupported=" << extensionsSupported << std::endl;
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        //[5]查询显卡特性，包括：纹理压缩，64位浮点述。多视口渲染（VR非常有用）
        //VkPhysicalDeviceFeatures deviceFeatures;
        //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //[5]是否为专业显卡（a dedicated graphics card ）（独显），或称为离散显卡,是否支持几何着色器
        //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        //or

        QueueFamilyIndices indices = findQueueFamilies(device);    //检测交换链是否支持
        //return indices.graphicsFamily.has_value();    //该值等于true
        std::cout << "isDeviceSuitable indices.graphicsFamily.has_value()=" << indices.graphicsFamily.has_value() << std::endl;
        //or
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }


    /// <summary>
    /// 查询交换链支持细节
    /// 只检查交换链是否可用还不够，交换链可能与我们的窗口表面不兼容。 创建交换链所要进行的设置要比 Vulkan 实例和设备创建多得多，在进行交换链创建之前需要我们查询更多的信息.有三种最基本的属性，需要我们检查：
    ///    1.基础表面特性(交换链的最小 / 最大图像数量，最小 / 最大图像宽度、高度)
    ///    2.表面格式(像素格式，颜色空间)
    ///    3.可用的呈现模式
    ///    与交换链信息查询有关的函数都需要VkPhysicalDevice 对象和 VkSurfaceKHR作为参数, 它们是交换链的核心组件        
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        //[5]basic surface capabilities 基本性能
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);//查询基础表面特性
        //[5]the supported surface formats
        
        //查询表面支持的格式,确保向量的空间足以容纳所有格式结构体
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

        }
        

        //[5]the supported presentation modes，查询支持的呈现模式
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;

    }
    //Vulkan所有的操作，从绘制到加载纹理都需要将操作指令提交给一个队列后才能执行，查找出满足我们需求的队列族。Vulkan 有多种不同类型的队列，它们属于不同的队列族，每个队列族的队列只允许执行特定的一部分指令
    
    /// <summary>
    /// 获取有图形能力的队列族，检测设备支持的队列族,查找出满足我们需求的队列族,这一函数会返回满足需求得队列族的索引，队列族是一组具有相同功能但能够并行运行的队列。队列族的数量，每个族的能力以及属于每个族的队列的数量都是物理设备的属性
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        //检测设备支持的队列族，以及它们中哪些支持我们需要使用的指令
        //[5]Logic to find graphics queue family
        QueueFamilyIndices indices;
        //[5]Logic to find queue family indices to populate struct with
        //[5]C++ 17引入了optional数据结构来区分存在或不存在的值的情况。
        std::optional<uint32_t> graphicsFamily;
        std::cout << "000 findQueueFamilies graphicsFamily.has_value() = " << std::boolalpha << graphicsFamily.has_value() << std::endl; // false
        graphicsFamily = 0;        //需要的队列族只需要支持图形指令即可
        std::cout << "111 findQueueFamilies graphicsFamily.has_value() = " << std::boolalpha << graphicsFamily.has_value() << std::endl; // true

        uint32_t queueFamilyCount = 0;
        //第一次调用获取设备的队列族个数，然后分配数组存储队列族的 VkQueueFamilyProperties 对象，第一个参数为In类型的物理设备，第二个为Out类型的，返回物理设备队列族的个数
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);    //获取设备的队列族个数
        std::cout << endl<<"本物理设备（显卡）的队列族个数：queueFamilyCount=" << queueFamilyCount << std::endl;
        
        //vkQueueFamilyProperties包含队列族的很多信息， 比如支持的操作类型，该队列族可以创建的队列个数
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        //VkQueueFamilyProperties结构体中queueFlags的参数解释见：https://blog.csdn.net/qq_36584063/article/details/71219188?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1.topblog&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1.topblog&utm_relevant_index=2

        //第二次调用，返回物理设备的队列族列表的属性
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        //我们需要找到至少一个支持VK_QUEUE_GRAPHICS_BIT的族，此系列中的队列支持图形操作，例如绘制点，线和三角形。 
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            //[5]寻找一个队列族，它能够链接window，查找带有呈现图像到窗口表面能力的队列族
            VkBool32 presentSupport = false;
            //检查物理设备是否具有呈现能力，查找带有呈现图像到窗口表面能力的队列族
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {    //根据队列族中的队列数量和是否支持表现确定使用的表现队列族的索引
                indices.presentFamily = i; 
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                //找到一个支持VK_QUEUE_GRAPHICS_BIT的队列族，VK_QUEUE_GRAPHICS_BIT表示支持图形指令
                indices.graphicsFamily = i;
                
                std::cout << "i=" << i <<"， queueFamily.queueCount="<<queueFamily.queueCount<<" ，" << std::endl;
                //说明：即使绘制指令队列族和呈现队列族是同一个队列族， 我们也按照它们是不同的队列族来对待。 显式地指定绘制和呈现队列族是同一个的物理设备来提高性能表现。
                if (indices.isComplete())
                {
                    break;
                }
                    
            }
            i++;
        }
        //std::cout<<indices.graphicsFamily.value <<std::endl;
        return indices;
    }

    //[5]
    
    /// <summary>
    /// 检测是否支持交换链，实际上，如果设备支持呈现队列，那么它就一定支持交换链。但我们最好还是显式地进行交换链扩展的检测，然后显式地启用交换链扩展。
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);    //枚举设备扩展列表，检测所需的扩展是否存在，，得到扩展的数量
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);        //定义扩展数组
        
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        int i = 0;
        for (const auto& extension : availableExtensions) {
            std::cout << "i=" << i << "，checkDeviceExtensionSupport extension.extensionName=" << extension.extensionName << std::endl;
            requiredExtensions.erase(extension.extensionName);
            i++;
        }
        bool empty = requiredExtensions.empty();
        std::cout << "requiredExtensions.empty()=" << empty << std::endl;
        return requiredExtensions.empty();
    }

    
    /// <summary>
    /// 给显卡评分，在多个显卡都合适的情况下，给每一个设备权重值，选择最高的一个
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    int rateDeviceSuitability(VkPhysicalDevice device) {
        //[5]查询显卡属性，包括：ID、类型、名称，支持Vulkan的版本号
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);    //输入显卡（物理设备），得到相应属性
        std::cout << std::endl
            << "deviceProperties.deviceID=" << deviceProperties.deviceID
            << " deviceProperties.vendorID=" << deviceProperties.vendorID
            << " deviceProperties.deviceType=" << deviceProperties.deviceType
            << "  deviceProperties.deviceName=" << deviceProperties.deviceName    //设备名称，我的是GeForce GTX 1660 Ti和AMD Radeon(TM) Graphics
            << "  deviceProperties.driverVersion=" << deviceProperties.driverVersion    //包含用于控制设备的驱动程序的版本
            << "  deviceProperties.apiVersion=" << deviceProperties.apiVersion    //设备支持的最高版本的Vulkan
            << std::endl;

        int score = 0;
        //[5]离散GPU具有显著的性能优势
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
            std::cout << "有离散GPU，分值增加1000" << std::endl;
        }
        else
        {
            std::cout << "无离散GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU（有集成GPU）， deviceProperties.deviceType=" << deviceProperties.deviceType << std::endl;
        }
        //[5]支持纹理的最大值，影响图形质量
        score += deviceProperties.limits.maxImageDimension2D;

        printf("支持的最大1D纹理值：deviceProperties.limits.maxImageDimension1D=%d\n", deviceProperties.limits.maxImageDimension1D);
        printf("支持的最大2D纹理值：deviceProperties.limits.maxImageDimension2D=%d\n", deviceProperties.limits.maxImageDimension2D);
        printf("支持的最大3D纹理值：deviceProperties.limits.maxImageDimension3D=%d\n", deviceProperties.limits.maxImageDimension3D);

        printf("支持的最大DimensionCube：deviceProperties.limits.maxImageDimensionCube=%d\n", deviceProperties.limits.maxImageDimensionCube);

        //[5]查询显卡特性，包括：纹理压缩，64位浮点述。多视口渲染（VR）
        VkPhysicalDeviceFeatures deviceFeatures;
        //第一个参数为In类型的，传入物理设备，第二个参数为Out类型的，返回设备特点的结构体
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        std::cout << "deviceFeatures.multiViewport=" << deviceFeatures.multiViewport
            << " deviceFeatures.textureCompressionETC2=" << deviceFeatures.textureCompressionETC2
            << " deviceFeatures.textureCompressionBC=" << deviceFeatures.textureCompressionBC
            << std::endl;
        //[5]不支持几何着色器
        if (!deviceFeatures.geometryShader) {
            score = 0;
            //return 0;
        }
        else
        {
            std::cout << "支持几何着色器 score=" << score << std::endl;
        }
        if (deviceFeatures.tessellationShader)
        {
            std::cout << "支持曲面细分着色器，tessellationShader" << std::endl;
        }
        return score;

    }
    

    /// <summary>
    /// 创建逻辑设备以作为物理设备的交互接口，逻辑设备用于描述可以使用的物理设备的特性。逻辑设备创建过程与instance创建过程类似，也需要描述我们需要使用的功能。因为我们已经查询过哪些队列簇可用，在这里需要进一步为逻辑设备创建具体类型的命令队列。如果有不同的需求，也可以基于同一个物理设备创建多个逻辑设备。
    /// </summary>
    void createLogicalDevice() {
        //获取带有图形能力的队列族
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        VkDeviceQueueCreateInfo queueCreateInfo = {};    //创建逻辑设备需要填写结构体，它描述了针对一个队列族我们所需的队列数量
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        //目前而言，对于每个队列族，驱动程序只允许创建很少数量的队列，但实际上，对于每一个队列族，我们很少需要一个以上的队列。我们可以在多个线程创建指令缓冲，然后在主线程一次将它们全部提交，降低调用开销。 Vulkan需要我们赋予队列一个0.0到1.0之间的浮点数作为优先级来控制指令缓冲的执行顺序。
        queueCreateInfo.queueCount = 1;
        //[6]Vulkan使用0.0到1.0之间的浮点数为队列分配优先级, 来进行缓冲区执行的调度。即使只有一个队列，这也是必需的，显式地赋予队列优先级：
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        //[6]device features，指定应用程序使用的设备特性
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};        //填写逻辑设备的结构体
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;    //pQueueCreateInfos 指针指向 queueCreateInfo 的地址
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;    //pEnabledFeatures 指针指向 deviceFeatures 的地址
        
        //[6]VK_KHR_swapchain 将该设备的渲染图像显示到windows
        //[6]之前版本Vulkan实现对实例和设备特定的验证层进行了区分，但现在情况不再如此。
        //[6]这意味着VkDeviceCreateInfo的enabledLayerCount和ppEnabledLayerNames字段被最新的实现忽略。不过，还是应该将它们设置为与较旧的实现兼容：
        createInfo.enabledExtensionCount = 0;
        if (enableValidationLayers) {
            //以对设备和 Vulkan 实例使用相同地校验层
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            std::cout << "createLogicalDevice()，有启用enableValidationLayers" << std::endl;
        }
        else {//进入了该分支
            createInfo.enabledLayerCount = 0;
            std::cout << "createLogicalDevice()，没有启用enableValidationLayers" << std::endl;
        }

        //[6]create a queue from both families
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            //描述了针对一个队列族我们所需的队列数量。填写[逻辑设备]的队列的构造信息
            VkDeviceQueueCreateInfo queueCreateInfo = {};    //创建所有使用到的队列族
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pNext = nullptr;    //我添加的
            queueCreateInfo.queueFamilyIndex = queueFamily;        //有图形能力的队列
            queueCreateInfo.queueCount = 1;        //申请使用队列的数量
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);  //push_back会负责将一个值当成vector对象的尾元素“压到（push）”vector对象的“尾端(back)”

        }
        //[6]将队列信息加入驱动info
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        //[6]开启扩展支持，启用交换链
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());    
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        //[6]创建逻辑设备，参数physicalDevice, &createInfo, nullptr为in类型的，device为Out类型的，返回逻辑设备
        /**        vkCreateDevice 函数的参数:
        1.创建的逻辑设备进行交互的物理设备对象
        2.指定的需要使用的队列信息
        3.可选的分配器回调，若设置为nullptr,则使用vulkan内置的内存分配器
        4.用来存储返回的逻辑设备对象的内存地址
        逻辑设备并不直接与Vulkan实例交互,所以创建逻辑设备时不需要使用Vulkan实例作为参数          */

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        //[6]获取驱动队列
        //获取指定队列族的队列句柄
        //它的参数依次是逻辑设备对象,队列族索引,队列索引,用来存储返回的队列句柄的内存地址
        //因为我们只从这个族中创建一个队列，所以我们只使用索引0
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        //[6]显示队列
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }
    //[6]----------------------------------------------------------------------------

    /// <summary>
    /// 交换链是渲染目标的集合，Vulkan 没有默认帧缓冲的概念，它需要一个能够缓冲渲染操作的组件。在 Vulkan 中，这一组件就是交换链。Vulkan 的交换链必须显式地创建，不存在默认的交换链。交换链本质上是一个包含了若干等待呈现的图像队列。我们的应用程序从交换链获取一张图像，然后在图像上进行渲染操作，完成后，将图像返回到交换链的队列中。交换链的队列的工作方式和它呈现图像到表面的条件依赖于交换链的设置。但通常来说，交换链被用来同步图像呈现和屏幕刷新
    /// </summary>
    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        //[7]简单地坚持这个最小值意味着我们有时可能需要等待驱动程序完成内部操作，
        //[7]然后才能获取另一个要渲染的图像。因此，建议请求至少比最小值多一个图像：
        //[7]uint32_t imageCount = swapChainSupport.capabilities.minImageCount;
         
        /**     设置包括交换链中的图像个数，也就是交换链的队列可以容纳的图像个数。 我们使用交换链支持的最小图像个数 +1 数量的图像来实现三倍缓冲。maxImageCount 的值为 0 表明，只要内存可以满足，我们可以使用任意数量的图像。
          */

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        //创建交换对象需要的结构体
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        //[7]imageArrayLayers指定每个图像包含的层的数量。除非您正在开发3D应用程序，否则该值始终为1。
        //指定每个图像所包含的层次.但对于 VR 相关的应用程序来说，会使用更多的层次
        createInfo.imageArrayLayers = 1;
        //指定我们将在图像上进行怎样的操作--这里是作为传输的目的图像
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        //[7]两种方法可以处理从多个队列访问的图像：
        //[7]VK_SHARING_MODE_CONCURRENT
        //[7]VK_SHARING_MODE_EXCLUSIVE
        cout << "in createSwapChain()" << endl;
        /**
       指定在多个队列族使用交换链图像的方式。这一设置对于图形队列和呈现队列不是同一个队列的情况有着很大影响。
       我们通过图形队列在交换链图像上进行绘制操作，然后将图像提交给呈现队列来显示。有两种控制在多个队列访问图像的方式:
       VK_SHARING_MODE_EXCLUSIVE：一张图像同一时间只能被一个队列族所拥有， 在另一队列族使用它之前，必须显式地改变图像所有权。这一模式下性能表现最佳。
       VK_SHARING_MODE_CONCURRENT：图像可以在多个队列族间使用，不需要显式地改变图像所有权。

       如果图形和呈现不是同一个队列族，我们使用协同模式来避免处理图像所有权问题。协同模式需要我们使用 queueFamilyIndexCount 和pQueueFamilyIndices 来指定共享所有权的队列族。
       如果图形队列族和呈现队列族是同一个队列族 (大部分情况下都是这样)， 我们就不能使用协同模式，协同模式需要我们指定至少两个不同的队列族。
       */
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        std::cout << "---------indices.graphicsFamily=" << indices.graphicsFamily.value() << "，indices.presentFamily=" << indices.presentFamily.value()<<", indices.graphicsFamily.has_value="<< indices.graphicsFamily.has_value() << ", indices.graphicsFamily.has_value()="<< indices.presentFamily.has_value() << std::endl;
        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;            
        }
        else {//进入该分支
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional            
        }
        
        //[7]指定对交换链中的图像应用某种变换，我们可以为交换链中的图像指定一个固定的变换操作(需要交换链具有supportedTransforms 特性)，比如顺时针旋转 90 度或是水平翻转。如果不需要进行任何变换操作，指定使用 currentTransform 变换即可。
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        //[7]alpha channel should be used for blending
        //成员变量用于指定 alpha 通道是否被用来和窗口系统中的其它窗口进行混合操作。 通常，我们将其设置为 VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR来忽略掉alpha通道。
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;        //设置呈现模式
        createInfo.clipped = VK_TRUE;        //设置为True时，表示我们不关心被窗口系统中的其它窗口遮挡的像素的颜色
        //[7]窗口重置是取缓存区图像方式，oldSwapchain需要指定它，是因为应用程序在运行过程中交换链可能会失效。 比如，改变窗口大小后，交换链需要重建，重建时需要之前的交换链
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        //创建交换链
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        /** 我们在创建交换链时指定了一个minImageCount成员变量来请求最小需要的交换链图像数量。 Vulkan 的具体实现可能会创建比这个最小交换链图像数量更多的交换链图像，我们在这里，我们仍然需要显式地查询交换链图像数量，确保不会出错。 */
        //获取交换链图像句柄
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        //存储我们设置的交换链图像格式和范围
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    //[7]，选择适当的表面格式
     /** 每一个 VkSurfaceFormatKHR 条目包含了一个 format 和 colorSpace成员变量
     *format 成员变量用于指定颜色通道和存储类型,如VK_FORMAT_B8G8R8A8_UNORM 表示我们以B，G，R 和 A 的顺序，    每个颜色通道用 8 位无符号整型数表示,总共每像素使用 32 位表示
         * colorSpace 成员变量用来表示 SRGB 颜色空间是否被支持，是否使用 VK_COLOR_SPACE_SRGB_NONLINEAR_KHR 标志。对于颜色空间,如果SRGB被支持,我们就使用SRGB,它可以得到更加准确的颜色表示
         * 这里使用VK_FORMAT_B8G8R8A8_UNORM表示RGB 作为颜色格式
         */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            //检测格式列表中是否有我们想要设定的格式是否存在
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        //[12]如果查询失败返回第一个，如果不能在列表中找到我们想要的格式，我们可以对列表中存在的格式进行打分，选择分数最高的那个作为我们使用的格式，当然，大多数情况下，直接使用列表中的第一个格式也是非常不错的选择。
        return availableFormats[0];
    }
    
    
    /// <summary>
    /// 查找最佳的可用呈现模式，呈现模式是交换链中最重要的设置，它决定了什么条件下图像才会显示到屏幕。Vulkan提供了4种可用的呈现模式：
    /// [7]VK_PRESENT_MODE_IMMEDIATE_KHR，应用程序提交的图像会被立即传输到屏幕上，可能会导致撕裂现象
    //  [7]VK_PRESENT_MODE_FIFO_KHR，保证一定可用交换链变成一个先进先出的队列，每次从队列头部取出一张图像进行显示，应用程序渲染的图像提交给交换链后，会被放在队列尾部。当队列为满时，应用程序需要进行等待。这一模式非常类似现在常用的垂直同步。刷新显示的时刻也被叫做垂直回扫。
    //  [7]VK_PRESENT_MODE_FIFO_RELAXED_KHR 这一模式和上一模式的唯一区别是，如果应用程序延迟，导致交换链的队列在上一次垂直回扫时为空， 那么，如果应用程序在下一次垂直回扫前提交图像，图像会立即被显示。这一模式可能会导致撕裂现象
    //  [7]VK_PRESENT_MODE_MAILBOX_KHR，表现最佳，这一模式是第二种模式的另一个变种。它不会在交换链的队列满时阻塞应用程序，队列中的图像会被直接替换为应用程序新提交的图像。这一模式可以用来实现三倍缓冲，避免撕裂现象的同时减小了延迟问题
    /// </summary>
    /// <param name="availablePresentModes"></param>
    /// <returns></returns>
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        //[7]三级缓存更好，如果有就开启
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }


    /// <summary>
    /// 在minImageExtent和maxImageExtent内选择与窗口最匹配的分辨率，设置交换范围。
    /// 交换范围是交换链中图像的分辨率，它几乎总是和我们要显示图像的窗口的分辨率相同。 VkSurfaceCapabilitiesKHR 结构体定义了可用的分辨率范围。 Vulkan 通过 currentExtent 成员变量来告知适合我们窗口的交换范围。 一些窗口系统会使用一个特殊值，uint32_t 变量类型的最大值， 表示允许我们自己选择对于窗口最合适的交换范围，但我们选择的交换范围需要在 minImageExtent 与 maxImageExtent 的范围内。
    /// 代码中 max 和 min 函数用于在允许的范围内选择交换范围的高度值和宽度值，需要在源文件中包含 algorithm 头文件才能够使用它们
    /// </summary>
    /// <param name="capabilities"></param>
    /// <returns></returns>
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            //[16]为了正确地处理窗口大小调整，还需要查询帧缓冲区的当前窗口大小，以确保交swap chain(new)中图像大小正确。
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

            //VkExtent2D actualExtent = { WIDTH, HEIGHT };
            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
            return actualExtent;
        }
    }

    //[7]--------------------------------------------------

    //[8]--------ImageView，图像视图用于指定图像的大小---------------------------------------

    void createImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());        //分配足够的数组空间来存储图像视图
        for (size_t i = 0; i < swapChainImages.size(); i++)        //遍历所有交换链图像，创建图像视图
        {//设置图像视图结构体相关信息
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            //[8]选择视图类型 1D 2D 3D
            //viewType 和 format 成员变量用于指定图像数据的解释方式。
            //viewType用于指定图像被看作是一维纹理、二维纹理、三维纹理还是立方体贴图
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            //[8]components字段允许旋转颜色通道。 
            //components 成员变量用于进行图像颜色通道的映射。比如，对于单色纹理，我们可以将所有颜色通道映射到红色通道。我们也可以直接将颜色通道的值映射为常数 0 或 1。在这里, 我们只使用默认的映射
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; //default
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            //[8]描述图像的用途以及应访问图像的哪个部分。
            //subresourceRange用于指定图像的用途和图像的哪一部分可以被访问
            //在这里，我们的图像被用作渲染目标，并且没有细分级别，只存在一个图层
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            //如果读者在编写 VR 一类的应用程序，可能会使用支持多个层次的交换链。这时，读者应该为每个图像创建多个图像视图，分别用来访问左眼和右眼两个不同的图层
            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
            {//创建图像视图，有了图像视图，就可以将图像作为纹理使用，但作为渲染目标，还需要帧缓冲对象
                throw std::runtime_error("failed to create image views!");
            }

        }
    }
    //[8]--------------------------------------------------------------------------------------------------------

    //[9]--------------------------------------------------------------------------------------------------------
    void createRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        //[9]colorAttachment的format应与swapChain图像的格式匹配
        colorAttachment.format = swapChainImageFormat;
        //[9]没有使用多重采样（multisampling），将使用1个样本。
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        //[9]clear the framebuffer to black before drawing a new frame
        //[9]VK_ATTACHMENT_LOAD_OP_LOAD 保留Attachment的现有内容
        //[9]VK_ATTACHMENT_LOAD_OP_CLEAR 开始时将值初始化为常数
        //[9]VK_ATTACHMENT_LOAD_OP_DONT_CARE 现有内容未定义； 忽略
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //[9]color and depth data
        //[9]VK_ATTACHMENT_STORE_OP_STORE 渲染的内容将存储在内存中，以后可以读取
        //[9]VK_ATTACHMENT_STORE_OP_DONT_CARE 渲染操作后，帧缓冲区的内容将是未定义的
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //[9]stencil data
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //[9]Textures and framebuffers 
        //[9]指定在渲染开始之前图像将具有的布局。
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //[9]指定在渲染完成时自动过渡到的布局。
        //[9]VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL used as color attachment
        //[9]VK_IMAGE_LAYOUT_PRESENT_SRC_KHR Images the swap chain 中要显示的图像
        //[9]VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL  用作存储器复制操作目的图像
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //[9]引用Attachment
        VkAttachmentReference colorAttachmentRef = {};
        //[9]attachment参数通过attachment描述数组中的索引指定要引用的attachment
        colorAttachmentRef.attachment = 0;
        //[9]布局指定了我们希望attachment在使用此引用的subpass中具有哪种布局。
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //[9]Vulkan may also support compute subpasses in the future
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        //[9]引用colorAttachment
        //[9]数组中attachment的索引是直接从fragment shader 引用的（location = 0）out vec4 outColor指令！
        //[9]subpass可以引用以下其他类型的attachment
        //[9]pInputAttachments read from a shader
        //[9]pResolveAttachments used for multisampling attachments
        //[9]pDepthStencilAttachment depth and stencil data
        //[9]pPreserveAttachments not used by this subpass but for which the data must be preserved(保存)
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        //[9]
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    //[9]----------------------------------------------------------------------------------------


    /// <summary>
    /// 10 图形管线：描述图形显卡分配的状态，例如视口大小和深度缓冲操作和着色器
    /////图形管线是一系列将我们提交的顶点和纹理转换为渲染目标上的像素的操作。它的简化过程如下：
    //    input assembler:获取顶点数据，顶点数据的来源可以是应用程序提交的原始顶点数据，或根据索引缓冲提取的顶点数据。
    //    vertex shader 对每个顶点进行模型空间到屏幕空间的变换，然后将顶点数据传递给图形管线的下一阶段。
    //    tessellation shaders 根据一定的规则对几何图形进行细分，从而提高网格质量。通常被用来使类似墙面这类不光滑表面看起来更自然。
    //    geometry shader 可以以图元(三角形，线段，点) 为单位处理几何图形，它可以剔除图元，输出图元。有点类似于 tessellation shader，但更灵活。但目前已经不推荐应用程序使用它，geometry shader 的性能在除了Intel 集成显卡外的大多数显卡上表现不佳。
    //    rasterization 阶段将图元离散为片段。片段被用来在帧缓冲上填充像素。位于屏幕外的片段会被丢弃，顶点着色器输出的顶点属性会在片段之间进行插值，开启深度测试后，位于其它片段之后的片段也会被丢弃。
    //    fragment shader 对每一个未被丢弃的片段进行处理，确定片段要写入的帧缓冲，它可以使用来自 vertex shader 的插值数据，比如纹理坐标和顶点法线。
    //    color blending 阶段对写入帧缓冲同一像素位置的不同片段进行混合操作。片段可以直接覆盖之前写入的片段，也可以基于之前片段写入的信息进行混合操作。
    //    原文链接：https ://blog.csdn.net/yuxing55555/article/details/88976664
    /// </summary>
    void createGraphicsPipeline()
    {
        auto vertShaderCode = readFile("vert.spv");
        auto fragShaderCode = readFile("frag.spv");
        //[10]
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
         
        //[10]可以将多个着色器组合到一个ShaderModule中，并使用不同的entry points来区分它们的行为。
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        //[10]指定着色器常量的值。 
        //[10]单个着色器模块，在管道中创建常量，给予不同的值来配置其行为。
        //[10]比在渲染时使用变量配置着色器更为有效， 编译器可以进行优化，例如消除依赖于这些值的if语句
        //[10]如果没有这样的常量，则可以将成员设置为nullptr，初始化会自动执行该操作。
        //[10]pSpecializationInfo    

        //[10]
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        //[10]
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        /**
          描述内容主要包括下面两个方面：
        绑定：数据之间的间距和数据是按逐顶点的方式还是按逐实例的方式进行组织
        属性描述：传递给顶点着色器的属性类型，用于将属性绑定到顶点着色器中的变量
        由于我们直接在顶点着色器中硬编码顶点数据，这里不载入任何顶点数据
          */

        //[10]//描述传递给顶点着色器的顶点数据格式--顶点输入
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        //[10]描述上述用于加载顶点数据的详细信息
        //用于指向描述顶点数据组织信息地结构体数组
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        //[10]描述上述用于加载顶点数据的详细信息
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        //[10]
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //[10]viewport
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        //[10]minDepth和maxDepth 在0.0f到1.0f之间
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        //[10]裁剪矩形定义哪些区域像素被存储
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;

        //[10]viewport 和scissor可以有多个
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        //[10]保留近平面和远平面之外的片元
        rasterizer.depthClampEnable = VK_FALSE;
        //[10]true 几何图形不会通过光栅化阶段。 禁用对帧缓冲区的任何输出
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        //[10]VK_POLYGON_MODE_FILL 用片段填充多边形区域
        //[10]VK_POLYGON_MODE_LINE 多边形边缘绘制为线
        //[10]VK_POLYGON_MODE_POINT 多边形顶点绘制为点
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        //[10]支持的最大线宽取决于硬件，任何比1.0f粗的线都需要启用wideLines。
        rasterizer.lineWidth = 1.0f;
        //[10]确定要使用的面部剔除类型。
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        //[10]指定面片视为正面的顶点顺序，可以是顺时针或逆时针
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        //[10]Rasterization可以通过添加常量或基于片段的斜率对深度值进行偏置来更改深度值。
        //[10]多用于阴影贴图，如不需要将depthBiasEnable设置为VK_FALSE。
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        //[10]
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        //[10]specification详见说明文档
        //[10]每个附加的帧缓冲区的混合规则 
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        //[6]片段着色器的颜色直接输出
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


        /*
            if (blendEnable) {
                finalColor.rgb = (srcColorBlendFactor * newColor.rgb)
                    < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
                finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp >
                    (dstAlphaBlendFactor * oldColor.a);
            }
            else {
                finalColor = newColor;

            }
            finalColor = finalColor & colorWriteMask;
        */
        //[10]透明混合
        /*
            finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
            finalColor.a = newAlpha.a;
        */
        //[10]VK_TRUE
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        //[10]全局颜色混合设置。
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        //[10]bitwise combination 请注意，这将自动禁用第一种方法
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        //[10]
        VkDynamicState dynamicStates[] = {
             VK_DYNAMIC_STATE_VIEWPORT,
             VK_DYNAMIC_STATE_LINE_WIDTH
        };

        //[10]
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;
        //[10]
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        //[10]
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        //[10]
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional

        pipelineInfo.layout = pipelineLayout;
        //[10]引用将使用图形管线的renderPass和subpass索引。
        //[10]也可以使用其他渲染管线，但是它们必须与renderPass兼容
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        //[10]Vulkan允许通过从现有管线中派生来创建新的图形管线。 
        //[10]管线派生的思想是，当管线具有与现有管线共有的许多功能时，建立管线的成本较低，
        //[10]并且可以更快地完成同一父管线之间的切换。 
        //[10]可以使用basePipelineHandle指定现有管线的句柄，也可以使用basePipelineIndex引用索引创建的另一个管线。
        //[10]现在只有一个管线，因此我们只需指定一个空句柄和一个无效索引。
        //[10]仅当在VkGraphicsPipelineCreateInfo的flags字段中还指定了VK_PIPELINE_CREATE_DERIVATIVE_BIT标志时，才使用这些值。
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional


        //[10]第二个参数VK_NULL_HANDLE引用了一个可选的VkPipelineCache对象。
        //[10]管线Cache可用于存储和重用管线创建相关的数据
        //[10]可以加快管线的创建速度,后有详解
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }


        //[10]
        //在创建图形管线之前，不会将SPIR-V字节码编译并链接到机器代码以供GPU执行。
        //这意味着在管道创建完成后，就可以立即销毁ShaderModule
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }


    VkShaderModule createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }

    //[10]--------------------------------------------------------------------------

    //[11]--------------------------------------------------------------------------
    /// <summary>
    /// 创建帧缓冲对象，帧缓冲区保存图像视图来指定使用的颜色，深度和模板
    /// </summary>
    void createFramebuffers()
    {
        //[11]调整容器大小以容纳所有帧缓冲区
        swapChainFramebuffers.resize(swapChainImageViews.size());    //分配足够的空间来存储所有帧缓冲对象

        //[11] create framebuffers
        for (size_t i = 0; i < swapChainImageViews.size(); i++) 
        { //为交换链的每一个图像视图对象创建对应的帧缓冲
            VkImageView attachments[] = { swapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            //指定帧缓冲需要兼容的渲染流程对象. 之后的渲染操作，我们可以使用与这个指定的渲染流程对象相兼容的其它渲染流程对象.一般来说，使用相同数量，相同类型附着的渲染流程对象是相兼容的。
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;    //指定附着个数
            framebufferInfo.pAttachments = attachments; //指定渲染流程对象用于描述附着信息的 pAttachment 数组
            //指定帧缓冲的大小
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;        //指定图像层数,这里的交换链图像都是单层的

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

    }
    //[11]-------------------------------------------------------------------------------

    //[12]-------------------------------------------------------------------------------
    //Vulkan 下的指令，比如绘制指令和内存传输指令并不是直接通过函数调用执行的。我们需要将所有要执行的操作记录在一个指令缓冲对象，然后提交给可以执行这些操作的队列才能执行。这使得我们可以在程序初始化时就准备好所有要指定的指令序列，在渲染时直接提交执行。也使得多线程提交指令变得更加容易。我们只需要在需要指定执行的使用，将指令缓冲对象提交给 Vulkan 处理接口。
    
    /// <summary>
    /// Command pool 分配command buffers，创建指令缓冲池
    /// 指令缓冲对象在被提交给我们之前获取的队列后，被 Vulkan 执行。每个指令池对象分配的指令缓冲对象只能提交给一个特定类型的队列。在这 里，我们使用的是绘制指令，它可以被提交给支持图形操作的队列。
    //有下面两种用于指令池对象创建的标记， 可以提供有用的信息给Vulkan的驱动程序进行一定优化处理：
    //VK_COMMAND_POOL_CREATE_TRANSIENT_BIT： 使用它分配的指令缓冲对象被频繁用来记录新的指令 (使用这一标记可能会改变帧缓冲对象的内存分配策略)
    //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT： 指令缓冲对象之间相互独立，不会被一起重置。 不使用这一标记，指令缓冲对象会被放在一起重置
    /// </summary>
    void createCommandPool() {
        std::cout << "in createCommandPool" << std::endl;
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo = {};    //指令池的创建只需要2个参数
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        //[12]There are two possible flags for command pools
        //[12]VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 提示CommandPool经常用新命令重新记录（可能会改变内存分配行为）
        //[12]VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 允许单独重新记录命CommandPool，否则都必须一起重置
        poolInfo.flags = 0; //可选的不使用flag

         //指令池对象的创建
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

    }

    //[13]---------------------------------------------------------------------------------------
    
    /// <summary>
    /// Command buffers用于记录指令操作，比如渲染
    /// 指令缓冲对象，用来记录绘制指令。由于绘制操作是在帧缓冲上进行的，我们需要为交换链中的每一个图像分配一个指令缓冲对象
    /// </summary>
    void createCommandBuffers() {
        commandBuffers.resize(swapChainFramebuffers.size()); //指定分配使用的指令池和需要分配的指令缓冲对象个数

        //VkCommandBuffer需要使用VkCommandPool来分配。我们可以为每个线程使用一个独立的VkCommandPool来避免进行同步，不同VkCommandPool使用自己的内存资源分配VkCommandBuffer。开始记录VkCommandBuffer后，调用的GPU指令，会被写入VkCommandBuffer。等待提交给队列执行
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        /**
        level 成员变量用于指定分配的指令缓冲对象是主要指令缓冲对象或者是辅助指令缓冲对象:
        VK_COMMAND_BUFFER_LEVEL_PRIMARY： 可以被提交到队列进行执行，但不能被其它指令缓冲对象调用。
        VK_COMMAND_BUFFER_LEVEL_SECONDARY： 不能直接被提交到队列进行执行，但可以被主要指令缓冲对象调用执行

        在这里，我们没有使用辅助指令缓冲对象，但辅助治理给缓冲对象的好处是显而易见的，我们可以把一些常用的指令存在辅助指令缓冲对象， 然后在主要指令缓冲对象中调用执行
          */
        //[13]指定primary 或 secondary command buffers.
    
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
        //分配指令缓冲对象
        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        //[13]recording a command buffer //记录指令到指令缓冲
        for (size_t i = 0; i < commandBuffers.size(); i++)
        {//指定一些有关指令缓冲的使用细节
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            /**
           flags 成员变量用于指定我们将要怎样使用指令缓冲。它的值可以是下面这些:
           VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT： 指令缓冲在执行一次后，就被用来记录新的指令.
           VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT： 这是一个只在一个渲染流程内使用的辅助指令缓冲.
           VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT： 在指令缓冲等待执行时，仍然可以提交这一指令缓冲
             */
            //[13]指定指定我们将如何使用command buffers.
            //[13]VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 在执行一次后立即rerecord。
            //[13]VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT 这是一个辅助command buffers, 将在单个渲染通道中使用
            //[13] VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT command buffer 可以在已经挂起执行时重新提交。
            beginInfo.flags = 0; // Optional
            //[13]仅与secondary command buffers 相关。 它指定从primarycommand buffers 继承哪些状态。
            //用于辅助指令缓冲，可以用它来指定从调用它的主要指令缓冲继承的状态
            beginInfo.pInheritanceInfo = nullptr; // Optional
            //指令缓冲对象记录指令后，调用vkBeginCommandBuffer函数会重置指令缓冲对象
            //开始指令缓冲的记录操作

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            //[13]Starting a render pass//指定使用的渲染流程对象
            VkRenderPassBeginInfo renderPassInfo = {};
            //[13]the render pass itself and the attachments to bind
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;          //指定使用的渲染流程对象
            renderPassInfo.framebuffer = swapChainFramebuffers[i];//指定使用的帧缓冲对象
            //[13]the size of the render area
            /**renderArea指定用于渲染的区域。位于这一区域外的像素数据会处于未定义状态。通常，我们将这一区域设置为和我们使用的附着大小完全一样.
              */
            renderPassInfo.renderArea.offset = { 0, 0 }; //It should match the size of the attachments for best performance
            renderPassInfo.renderArea.extent = swapChainExtent;
            /**
            所有可以记录指令到指令缓冲的函数的函数名都带有一个 vkCmd 前缀， 并且这些函数的返回值都是 void，也就是说在指令记录操作完全结束前， 不用进行任何错误处理。
            这类函数的第一个参数是用于记录指令的指令缓冲对象。第二个参数 是使用的渲染流程的信息。最后一个参数是用来指定渲染流程如何提供绘 制指令的标记，它可以是下面这两个值之一：
            VK_SUBPASS_CONTENTS_INLINE： 所有要执行的指令都在主要指令缓冲中，没有辅助指令缓冲需要执行
            VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS： 有来自辅助指令缓冲的指令需要执行。
              */
            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            renderPassInfo.clearValueCount = 1;     //指定标记后，使用的清除值
            renderPassInfo.pClearValues = &clearColor;
            //[13]最后一个参数：how the drawing commands within the render pass will be provided
            //[13]VK_SUBPASS_CONTENTS_INLINE render pass commands 将嵌入 primary command buffer, 并且不会执行 secondary command buffers
            //[13]VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERSrender pass commands 将从 secondary command buffers 执行
            //开始一个渲染流程
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            //凡是以vkCmd开头的调用都是录制（Record）命令，凡是以vkQueue开头的都是提交（Submit）命令，
            //[13]Basic drawing commands
            //[13]第二个参数指定管线对象是图形管线还是计算管线。
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            //[13]vertexCount 即使没有顶点缓冲区，从技术上讲，仍然有3个顶点要绘制
            //[13]instanceCount 用于实例化渲染，如果不进行实例化使用1
            //[13]firstVertex 顶点缓冲区的偏移量，定义gl_VertexIndex的最小值
            //[13]firstInstance 用作实例渲染的偏移量，定义gl_InstanceIndex的最小值

              /**            至此，我们已经提交了需要图形管线执行的指令，以及片段着色器使用的附着            */
            /**
              vkCmdDraw参数：
              1.记录有要执行的指令的指令缓冲对象
              2. vertexCount：
                尽管这里我们没有使用顶点缓冲，但仍然需要指定三个顶点用于三角形的绘制。
              3.instanceCount：用于实例渲染，为 1 时表示不进行实例渲染
              4.firstVertex：用于定义着色器变量 gl_VertexIndex 的值
              5.firstInstance：用于定义着色器变量 gl_InstanceIndex 的值
              */
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);//开始调用指令进行三角形的绘制操作

            //[13]The render pass can now be ended
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) { //结束记录指令到指令缓冲
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    //[13]---------------------------------------------------------------------------

    //[14]---------------------------------------------------------------------------
    void createSemaphores()
    {
        //创建每一帧需要的信号量对象
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        //[14]
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        //[15]
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);


        //[14] Vulkan API 会扩展 flags 和 pNext 参数。
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        //[14]
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        /**
        默认情况下，栅栏 (fence) 对象在创建后是未发出信号的状态。 这就意味着如果我们没有在 vkWaitForFences 函数调用
        之前发出栅栏 (fence) 信号，vkWaitForFences 函数调用将会一直处于等待状态。 这里设置初始状态为已发出信号
          */
        //[14]if we had rendered an initial frame that finished
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        //[14]
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {//创建信号量和VkFence 对象
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                //[14]
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphores for a frame!");
            }
        }

    }


    //[15]-----------------------异步执行的--从交换链获取图像-------------------------------------------------
    //[15]acquire an image from the swap chain

    /// <summary>
    /// * 1.从交换链获取一张图像
    /// *2.对帧缓冲附着执行指令缓冲中的渲染指令
    ///    * 3.返回渲染后的图像到交换链进行呈现操作
        //上面这些操作每一个都是通过一个函数调用设置的, 但每个操作的实际执行却是异步进行的。函数调用会在操作实际结束前返回，并且操作的实
        //际执行顺序也是不确定的。而我们需要操作的执行能按照一定的顺序，所以就需要进行同步操作。
        //有两种用于同步交换链事件的方式：栅栏(fence) 和信号量(semaphore)。它们都可以完成同步操作。
        //栅栏(fence) 和信号量(semaphore) 的不同之处是，我们可以通过调用 vkWaitForFences 函数查询栅栏(fence) 的状态，但不能查询信号量
        //(semaphore) 的状态。
        // 通常，我们使用栅栏(fence) 来对应用程序本身和渲染操作进行同步。
        // 使用信号量(semaphore) 来对一个指令队列内的操作或多个不同指令队列的操作进行同步。
        //这里，我们想要通过指令队列中的绘制操作和呈现操作，显然，使用信号量(semaphore) 更加合适。
    /// </summary>
    void drawFrame() {
        //[15]wait for the frame to be finished
        //[15]vkWaitForFences函数接受一个fences数组，并等待其中任何一个或所有fences在返回之前发出信号。
        //[15]这里传递的VK_TRUE表示要等待所有的fences，但是对于单个fences来说，这显然无关紧要。
        //[15]就像vkAcquireNextImageKHR一样，这个函数也需要超时。与信号量不同，我们需要通过vkresetfines调用将fence重置为unsignaled状态。
        //vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        //or    

        uint32_t imageIndex;
        //[15]第三个参数指定可获得图像的超时时间（以纳秒为单位）。
        //[15]接下来的两个参数指定了同步对象，这些对象将在引擎使用完图像时发出信号。 可以指定semaphore、fence或都指定
        //[15]用于输出可用swap chain中图像的索引。
        //vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        //or
        //[15]
        //vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        //or
        //[16]
        /**
          vkAcquireNextImageKHR参数：
          1.使用的逻辑设备对象
          2.我们要获取图像的交换链，
          3.图像获取的超时时间，我们可以通过使用无符号 64 位整型所能表示的
            最大整数来禁用图像获取超时
          4,5.指定图像可用后通知的同步对象.可以指定一个信号量对象或栅栏对象，
            或是同时指定信号量和栅栏对象进行同步操作。
            在这里，我们指定了一个叫做 imageAvailableSemaphore 的信号量对象
          6.用于输出可用的交换链图像的索引，我们使用这个索引来引用我们的
            swapChainImages数组中的VkImage对象，并使用这一索引来提交对应的指令缓冲
          */
          //从交换链获取一张图像，交换链是一个扩展特性，所以与它相关的操作都会有 KHR 这一扩展后缀
        
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);     //从交换链获取图片
        //[16]VK_ERROR_OUT_OF_DATE_KHR: Swap chain 与 surface 不兼容，无法再用于渲染。 通常发生在窗口调整大小之后
        //[16]VK_SUBOPTIMAL_KHR Swap:  chain仍可用于成功呈现到surface，但surface属性不再完全匹配
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //[18]如果SwapChain在尝试获取图像时已过时，则无法再显示给它。因此，我们应该立即重新创建SwapChain，并在下一个drawFrame调用中重试。
            recreateSwapChain();
            return;
        }
        //[16]如果SwapChain是次优的，可以调用recreateSwapChain，但还是选择继续，因为我们已经获得了一个图像。VK_SUCCESS和VK_SUBOPTIMAL_KHR都被认为是“成功”    。
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        /**
        vkWaitForFences 函数可以用来等待一组栅栏 (fence) 中的一个或全部栅栏 (fence) 发出信号。上面代码中我们对它使用的 VK_TRUE
        参数用来指定它等待所有在数组中指定的栅栏 (fence)。我们现在只有一个栅栏 (fence) 需要等待，所以不使用 VK_TRUE 也是可以的。和
        vkAcquireNextImageKHR 函数一样，vkWaitForFences 函数也有一个超时参数。和信号量不同，等待栅栏发出信号后，我们需要调用 vkResetFences
        函数手动将栅栏 (fence) 重置为未发出信号的状态。
          */

        //[15]Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);    //等待我们当前帧所使用的指令缓冲结束执行
        }
        //[15] Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        //提交信息给指令队列
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
        //or
        //[15]
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        //这里需要写入颜色数据到图像,所以我们指定等待图像管线到达可以写入颜色附着的管线阶段
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        //[15]前三个参数指定在执行开始之前等待哪些Semaphore，以及在管线的哪个阶段等待。
        /**
        waitSemaphoreCount、pWaitSemaphores 和pWaitDstStageMask 成员变量用于指定队列开始执行前需要等待的信号量，以及需要等待的管线阶段
          */
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;    //waitStages 数组中的条目和 pWaitSemaphores 中相同索引的信号量相对应。

        //[15]指定实际提交执行的commandBuffer。
        //指定实际被提交执行的指令缓冲对象
        //我们应该提交和我们刚刚获取的交换链图像相对应的指令缓冲对象
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        //[15]指定在commandBuffer完成执行后发送信号的Semaphore。
        //VkSemaphore signalSemaphores[] = { renderFinishedSemaphore};
        //or
        //[15]
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        //指定实际被提交执行的指令缓冲对象
        //我们应该提交和我们刚刚获取的交换链图像相对应的指令缓冲对象
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        //[15] 
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        //[15]最后一个参数引用了可选的 fenc, 当 command buffer 执行完成时，它将发出信号。
        //[15]我们使用信号量进行同步，所以我们传递VK_NULL_HANDLE。
        //if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to submit draw command buffer!");
        //}
        //or
        //凡是以vkCmd开头的调用都是录制（Record）命令，凡是以vkQueue开头的都是提交（Submit）命令，
         /**
        vkQueueSubmit 函数使用vkQueueSubmit结构体数组作为参数,可以同时大批量提交数.。
        vkQueueSubmit 函数的最后一个参数是一个可选的栅栏对象，
        可以用它同步提交的指令缓冲执行结束后要进行的操作。
        在这里，我们使用信号量进行同步，没有使用它，将其设置为VK_NULL_HANDLE
          */

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
        {//选择适当的Command buffers提交给Queue，提交指令缓冲给图形指令队列
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        
        //将渲染的图像返回给交换链进行呈现操作
        VkPresentInfoKHR presentInfo = {};    //配置呈现信息
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        //[15]在呈现（Present）之前等待哪些信号量
        presentInfo.waitSemaphoreCount = 1;        //指定开始呈现操作需要等待的信号量
        presentInfo.pWaitSemaphores = signalSemaphores;
        
        //指定了用于呈现图像的交换链，以及需要呈现的图像在交换链中的索引
        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        //[15]为每个swapChain指定呈现的图像和图像索引
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        //[15]check for every individual swap chain if presentation was successful. （array of VkResult）
         /**
        我们可以通过 pResults 成员变量获取每个交换链的呈现操作是否成功的信息。在这里，由于我们只使用了一个交换链，可以直接使用呈现函数
        的返回值来判断呈现操作是否成功
          */
        presentInfo.pResults = nullptr; // Optional

        //[15]submits the request to present an image to the swap chain.
        //vkQueuePresentKHR(presentQueue, &presentInfo);
        //or
        //[16]vkQueuePresentKHR 函数返回具有相同vkAcquireNextImageKHR返回值含义相同的值。
        //[16]在这种情况下，如果SwapChain不是最优的，我们也会重新创建它，因为我们想要最好的结果。
        result = vkQueuePresentKHR(presentQueue, &presentInfo);         //请求交换链进行图像呈现操作

        //添加 framebufferResized 确保semaphores处于一致状态，否则可能永远不会正确等待发出信号的semaphores。 
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
        /**
        如果开启校验层后运行程序，观察应用程序的内存使用情况， 可以发现我们的应用程序的内存使用量一直在慢慢增加。这是由于我
        们的 drawFrame 函数以很快地速度提交指令，但却没有在下一次指令提交时检查上一次提交的指令是否已经执行结束。也就是说 CPU 提交
        指令快过 GPU 对指令的处理速度，造成 GPU 需要处理的指令大量堆积。更糟糕的是这种情况下，我们实际上对多个帧同时使用了相同的
        imageAvailableSemaphore 和 renderFinishedSemaphore 信号量。
        最简单的解决上面这一问题的方法是使用 vkQueueWaitIdle 函数来等待上一次提交的指令结束执行，再提交下一帧的指令：
        但这样做，是对 GPU 计算资源的大大浪费。图形管线可能大部分时间都处于空闲状态.
          */


        //[15]        //更新currentFrame
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        //[15]
        vkQueueWaitIdle(presentQueue);         //等待一个特定指令队列结束执行

    }


    //[16]recreate 之前先 cleanup
    void cleanupSwapChain() {
        for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
        {//销毁帧缓冲
            vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
        }
        //[16]可以重新创建Command池，但相当浪费的。相反，使用vkfreecandbuffers函数清理现有CommandBuffer。就可以重用现有的池来分配新的CommandBuffer。
        //清除分配的指令缓冲对象
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());        //释放资源
        vkDestroyPipeline(device, graphicsPipeline, nullptr);        //销毁管线对象
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);    //销毁管线布局对象
        vkDestroyRenderPass(device, renderPass, nullptr);            //销毁渲染流程对象
        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {//销毁图像视图
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);        //销毁交换链对象，在逻辑设备被清除前调用
    }

    //[16] recreate SwapChain， pipeline must rebuilt
    void recreateSwapChain() {
        //[16]处理最小化
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {//设置应用程序在窗口最小化后停止渲染，直到窗口重新可见时重建交换链
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        //[16]等待资源完成使用。
        vkDeviceWaitIdle(device); //等待设备处于空闲状态，避免在对象的使用过程中将其清除重建
        cleanupSwapChain();        //清除交换链相关
        createSwapChain();        //重新创建交换链
        createImageViews();        //图形视图是直接依赖于交换链图像的，所以也需要被重建
        createRenderPass();        //渲染流程依赖于交换链图像的格式,窗口大小改变不会引起使用的交换链图像格式改变
        //视口和裁剪矩形在管线创建时被指定，窗口大小改变，这些设置也需要修改
        //我们可以通过使用动态状态来设置视口和裁剪矩形来避免重建管线
        createGraphicsPipeline();
        //帧缓冲和指令缓冲直接依赖于交换链图像
        createFramebuffers();
        createCommandBuffers();
    }

    //[16]  为静态函数才能将其用作回调函数
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        //[16-2]当前对象指针赋值
        auto app = reinterpret_cast<HelloTriangle*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

};


int main() {
    HelloTriangle app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

//处理所有的输入，查询glfw看是否有相关的键被按下或松开，并作出相应的响应处理
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}