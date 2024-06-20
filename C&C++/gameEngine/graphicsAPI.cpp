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
//[2]��֤��Debugʱ����
#ifdef NDEBUG    //������
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
//[2]�������õı�׼��֤������SDK��һ�����У���ΪVK_LAYER_KHRONOS_validation�㡣
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

//[5]��ʾ��������Ķ�����
struct QueueFamilyIndices {
    //���Ǿ����������������:���ǿ��ܷ���/����/ʹ��ĳ�����͵Ķ���Ҳ����˵�����ǿ�����ĳ�����͵�ֵ��Ҳ����û���κ�ֵ����ˣ�������Ҫһ�ַ�����ģ������ָ������壬��ָ���У����ǿ���ʹ��nullptr����ʾû��ֵ�������������ķ����Ƕ���һ���ض����͵Ķ��󣬲���һ������Ĳ�����Ա / ��־����ʾֵ�Ƿ���ڡ�std::optional<>��һ�����Ͱ�ȫ�ķ�ʽ�ṩ�������Ķ���std::optional����ֻ�ǰ���������ڲ��ڴ����һ��������־����ˣ���Сͨ���Ȱ����Ķ����һ���ֽ�

    std::optional<uint32_t> graphicsFamily;        //���ֶα�ʾ��������Ķ�����
    std::optional<uint32_t> presentFamily;        //֧�ֱ��ֵĶ���������
    //[8]Ϊ�˷�����������ǻ�����ṹ�������һ�����ͼ��
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

//[5]�豸��չ�б����VK_KHR_swapchain
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
    //[10]ate �������ǣ����Ի�ȡ�ļ��Ĵ�С
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    //[10]ָ������ͷ
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}


//[14]
const int MAX_FRAMES_IN_FLIGHT = 2;        //����ͬʱ���д����֡��


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
    VkInstance instance;    //����instance
    //[3]
    VkSurfaceKHR surface;    //���ڱ���
    //[4]
    VkDebugUtilsMessengerEXT debugMessenger;
    //[5]��vkinInstance������ʱ���ö��󽫱���ʽ���٣���˲���Ҫ��cleanup������ִ�����١�
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    //[6]
    VkDevice device;    //�߼��豸������Ϊ���Ա
    //[6]
    VkQueue graphicsQueue;
    //[6]
    VkQueue presentQueue;    //���ֶ���
    //[7]
    VkSwapchainKHR swapChain;    //������
    //[7]
    std::vector<VkImage> swapChainImages;    //��������ͼ����, �ڽ��������ʱ�Զ������
    //[7]
    VkFormat swapChainImageFormat;        //������ͼ���ʽ
    //[7]
    VkExtent2D swapChainExtent;            //������ͼ��Χ
    //[8]
    std::vector<VkImageView> swapChainImageViews;    //�洢ͼ����ͼ
    //[9]
    VkRenderPass renderPass;
    //[10]
    VkPipelineLayout pipelineLayout;
    //[10]
    VkPipeline graphicsPipeline;
    //[11]
    std::vector<VkFramebuffer> swapChainFramebuffers;        //�洢����֡�������
    //[12]
    VkCommandPool commandPool;    //����ָ��ض��� //ָ��ض���,����ָ������ʹ�õ��ڴ棬������ָ������ķ���

    //[13] Command buffers will be automatically freed when their command pool is destroyed
    std::vector<VkCommandBuffer> commandBuffers;    //ָ��ض���,����ָ������ʹ�õ��ڴ棬������ָ������ķ���

    //[14]Ϊÿһ֡�������������Լ����ź������ź�������ͼ���Ѿ�����ȡ�����Կ�ʼ��Ⱦ���ź�
    std::vector<VkSemaphore> imageAvailableSemaphores;
    //[14]�ź���������Ⱦ�Ѿ��н�������Կ�ʼ���ֵ��ź�
    std::vector<VkSemaphore> renderFinishedSemaphores;
    //[14] ��Ҫʹ��դ��(fence) ������ CPU �� GPU ֮���ͬ���� ����ֹ�г��� MAX_FRAMES_IN_FLIGHT֡��ָ��ͬʱ���ύִ�С�
    //    դ��(fence) ���ź���(semaphore) ���ƣ��������������źź͵ȴ��ź�
    std::vector<VkFence> inFlightFences;        //ÿһ֡����һ��VkFence դ������
    //[14]-[15]�Ƿ���Ҫ�ͷţ�
    std::vector<VkFence> imagesInFlight;

    //[15]
    size_t currentFrame = 0;          //׷�ٵ�ǰ��Ⱦ������һ֡

    //[16]
    bool framebufferResized = false;    //��Ǵ��ڴ�С�Ƿ����ı�

    //[1]
    void initWindow() {
        glfwInit();        //��ʼ��glfw��
        //��Ϊglfw�����Ϊ����OpenGL�����Ķ���Ƶģ���������Ҫ��������Ҫͨ���������ô���OpenGL�����ģ�GLFW_NO_API
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);        //���ڴ�С����Ϊ���ɱ�
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Study Yue Qingxuan", nullptr, nullptr);    //��������
        //[16-1]���浱ǰ����ָ��
        glfwSetWindowUserPointer(window, this);
        //[1] ��ⴰ��ʵ�ʴ�С�����ǿ���ʹ�� GLFW ����е� glfwSetFramebufferSizeCallback ���������ûص���
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
        //[16]��������ִ��ͬ���Ļ����ķ���.
        vkDeviceWaitIdle(device);
    }

    //VkInstance Ӧ����Ӧ�ó������ǰ�����������
    void cleanup()
    {
        //[17]
        cleanupSwapChain();//���ٽ������������߼��豸�����ǰ����

        //[14]        //���Ϊÿһ֡�������ź�����VkFence
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {//��������ͬ����ָ��ִ�н����󣬶����������
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
        //17����Щ�ͷ���Դ�ĺ����ƶ���cleanupSwapChain��
        //[12]
        //vkDestroyCommandPool(device, commandPool, nullptr);    //���������ָ��ض���
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
        //[6]�߼��豸����ֱ���� Vulkan ʵ�����������Դ����߼��豸ʱ����Ҫʹ�� Vulkan ʵ����Ϊ����
        vkDestroyDevice(device, nullptr);    //����߼��豸����
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
        vkDestroySurfaceKHR(instance, surface, nullptr);//���ٴ��ڱ�����󣬱������������Ҫ�� Vulkan ʵ�������֮ǰ���
        //[2]
        vkDestroyInstance(instance, nullptr);    //����Vulkanʵ��
        //[1]
        glfwDestroyWindow(window);    //���ٴ���
        glfwTerminate();    //����glfw
    }


    /// <summary>
    ///[2]����ʵ����ʵ��������Ӧ�ó���Ϳ���֧�ֵ�API��չ������VkApplicationInfo������Ӧ�ó������ơ���������
    /// VkInstanceCreateInfo������VkInstance������ȫ�ֵ���չ����֤�㣬��һ����չ���ڴ�������Ҫ����չ������ʵ���ſ���ѡ�������豸
    /// </summary>
    void createInstance() {
        //[2]�������Ƿ�������֤�㣬����enableValidationLayers=false��checkValidationLayerSupport()=true
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        else
        {
            cout << "createInstance enableValidationLayers=" << enableValidationLayers << ", !checkValidationLayerSupport()= " << !checkValidationLayerSupport() << std::endl<<"��������" << std::endl;
        }
        // ����һ��ʵ�����ȱ�����дһ�������й�����Ӧ�ó������Ϣ�Ľṹ: VkApplicationInfo
        // ��Щ�����ڼ����ϲ��Ǳ���ģ����ǿ�ѡ�ģ�������Ϊ���������ṩһЩ���õ���Ϣ���Ա�������ǵ��ض�Ӧ�ý����Ż�
        //[2]well-known graphics engine 
        VkApplicationInfo appInfo = {};    //������������Ӧ�ó�����Ϣ�Ľṹ��
        //[2]�ṹ�����ָ�����ͣ�pNextָ����չ��Ϣ
        // Vulkan�е����ṹҪ����sType��Ա����ȷָ�����ͣ���Ҳ�Ǿ���pNext��Ա�����ṹ�е�һ�����ó�Ա�����ڽ���ָ����չ��Ϣ��
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;    //�������Լ���ӵ�
        appInfo.pApplicationName = "Hello Triangle";
        //appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
        appInfo.pEngineName = "No Engine";
        //appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        //appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        //[2]Vulkan��������ʹ����Щȫ����չ����֤����������ϸ˵�� 
        // Vulkan�еĺܶ���Ϣ����ͨ���ṹ�����Ǻ����������ݵģ����Ǳ��������һ���ṹ�� VkInstanceCreateInfo ��Ϊ����ʵ���ṩ�㹻����Ϣ��VkInstanceCreateInfo�ṹ�Ǳ���ָ���ģ�������Vulkan��������������Ҫʹ����Щȫ����չ����֤�㡣
        VkInstanceCreateInfo createInfo = {};
        //����ÿ��Vulkan�ṹ��������API���ݲ����ĵ�һ����Ա��sType�ֶΣ�������Vulkan����ʲô���͵Ľṹ��sType�ֶξ���һ��Լ���׳ɵ���·,���Զ���������sType��ֵ
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;    //���ڷ�����������Ӧ�ó�����Ϣ�Ľṹ��(�ĵ�ַ)


        //[2]ָ��ȫ����չ
        // Vulkan��һ����ƽ̨�޹ص�API������ζ����Ҫһ���봰��ϵͳ�ӿڵ���չ��
        // GLFW��һ����������ú���������������Ҫ������չ�����ǿ��Դ��ݸ��ṹ�壺VkInstanceCreateInfo
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // �˺�������GLFW�����Vulkanʵ����չ�������飬�Ա�ΪGLFW���ڴ���Vulkan surface��
       // ����ɹ����б�ʼ�հ���`VK_KHR_surface`��������������Ҫ�κ�������չ������Խ����б�ֱ�Ӵ��ݸ�`VkInstanceCreateInfo`�ṹ��
       // ���������û��Vulkan����˺������ء�NULL�������� GLFW_API_UNAVAILABLE����

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        //[2]the global validation layers to enable
        createInfo.enabledLayerCount = 0; //������˵��
        //[2]��֤����Ϣ
        //[2]������ɹ�����ôvkCreateInstance���᷵��VK_ERROR_LAYER_NOT_PRESENT����
        if (enableValidationLayers) {
            // �ṹ������������Աȷ��Ҫ���õ�ȫ����֤��
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

        //[2]����
        //[2]ͨ���÷�ʽ����һ������ĵ�����Ϣ��������vkCreateInstance��vkDestroyInstance�ڼ��Զ�����������
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
        //[2]����ʵ��    
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance������Vulkanʵ��ʧ��!");
            /*
            * //[2]��֤��˵����Vulkanÿ�ε��ö��������Ӧ����֤��ͨ������ֵ�ж������Ƿ�ִ�гɹ�
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
        //[2]֧����չ������
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        //[2]an array of VkExtensionProperties to store details of the extensions.
        //[2]an array to hold the extension details
        //[2]֧�ֵ���չ��ϸ��Ϣ������֧�ֵ���չ�б�
        std::vector<VkExtensionProperties> extensionsProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProperties.data());

        //[2]query the extension details
        //[2]Each VkExtensionProperties struct contains the name and version of an extension.
        //[2]��ѯ��չ����ϸ��Ϣ
        std::cout << "createInstance�����ؿ�֧�ֵ���չ�б�available extensions:" << std::endl;
        std::cout<<"��ǰ�ļ���Ϊ��"<<__FILE__<<"���к�Ϊ��"<<__LINE__<<std::endl;
        int i = 1;
        for (const auto& extension : extensionsProperties) {
            std::cout << i << "," << "\t" << extension.extensionName << std::endl;
            i++;
        }
        std::cout << "��ǰ�ļ���Ϊ��" << __FILE__ << "���к�Ϊ��" << __LINE__ << std::endl;
    }

    //[2]list all of the available layers
    //[2]�г�������֤�����Ϣ���������п��õ�У���
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        //��һ�ε��ã�ȷ���������
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);        //���������С
        //�ڶ��ε��ã���������
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        //[2]��ѯ[vulkanʵ��]���Ĳ㣬�Ƿ������֤����Ϣ layerName = VK_LAYER_KHRONOS_validation
        for (const char* layerName : validationLayers) {
            bool layerFound = false;
            int i = 1;
            for (const auto& layerProperties : availableLayers) {
                std::cout << std::endl << "i=" << i << "��layerName=" << layerName << ", layerProperties.layerName=" << layerProperties.layerName;
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    std::cout << std::endl << "layerName=" << layerName << ",layerProperties.layerName=" << layerProperties.layerName << "�������ҵ�layer��layerFound����true" << std::endl;
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
    //[2]���Ǳ���ʹ��VK_EXT_debug_utils��չ������һ�����лص���debug messenger�����ûص����������ܵ�����Ϣ��
    //�������Ĺ��ܣ��Ƿ����������֤�㣬�����������չ���б�
    std::vector<const char*> getRequiredExtensions() {
        //[5]ָ��GLFW��չ������debug messenger ��չ����������ӵ�
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::cout << "* glfwExtensions=" << *glfwExtensions << std::endl;
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {//������֤��
            //[2]������ʹ��VK_EXT_DEBUG_UTILS_EXTENSION_NAME�꣬�������ַ�����VK_EXT_debug_utils����
            //[2]ʹ�ô˺���Ա����������push_back�Ḻ��һ��ֵ����vector�����βԪ�ء�ѹ����push����vector����ġ�β��(back)��
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        else
        {
            std::cout << "ϵͳû������У��㣬enableValidationLayers=" << enableValidationLayers << std::endl;
        }
        std::cout<< "extensions.size()="<< extensions.size() <<std::endl;
        return extensions;
    }

    //[2]��ϸ�Ķ���չ�ĵ����ͻᷢ����һ�ַ�������ר��Ϊ�������������ô��������� debug utils messenger.
    //[2]��Ҫ����ֻ����VkInstanceCreateInfo��pNext��չ�ֶ���
    //[2]����һ��ָ��VkDebugUtilsMessengerCreateInfoEXT�ṹ��ָ�롣
    //[2]���Ƚ�messenger������Ϣ�������ȡ�������ĺ����У�
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |    //�����Ϣ
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |    //������Ϣ
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;        //���Ϸ��Ϳ�����ɱ����Ĳ�����Ϣ
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    //[2]Add a new static member function called debugCallback with 
    //[2]    the PFN_vkDebugUtilsMessengerCallbackEXT prototype.
    //[2]ʹ��PFN_vkDebugUtilsMessengerCallbackEXT�������һ����̬����
    //[2]The VKAPI_ATTR and VKAPI_CALL ensure that the function has the
    //[2]    right signature for Vulkan to call it.
    //[2]ʹ��VKAPI_ATTR��VKAPI_CALL ȷ������������ȷ��ǩ�����Ա�Vulkan������
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT �����Ϣ
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ��Ϣ����Ϣ������Դ�Ĵ���
        //[2]������Ϊ����Ϣ,�䲻һ���Ǵ��󣬵��ܿ�����Ӧ�ó����е�BUG
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        //[2]������Ч�ҿ��ܵ��±�������Ϊ����Ϣ
        //[2]VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
        //[2]����ʹ�ñȽϲ����������Ϣ�Ƿ���ĳ�������Լ�����Ȼ������磺
        //[2]if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        //[2]    // Message is important enough to show
        //[2]}        
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,    //ָ������Ϣ�����س̶�
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ������һЩ��淶�������޹ص��¼�
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ������Υ���淶��һ��������ʾ�Ĵ���
        //[2]VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT �����ŵķ�ʽʹ��Vulkan�������˿���Ӱ��Vulkan���ܵ���Ϊ
        VkDebugUtilsMessageTypeFlagsEXT messageType,        //ָ������Ϣ��Ϣ������


        //[2]��Ϣ�������ϸ��Ϣ, ��������Ҫ��Ա��
        //[2]pMessage ��null��β�ĵ�����Ϣ�ַ���
        //[2]pObjects ����Ϣ��ص�Vulkan����������
        //[2]objectCount �����еĶ������
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,    //�ýṹ���������Ϣ�����ϸ������
        //���һ������pUserData �����ڻص������ڼ�ָ����ָ�룬�����������Լ������ݴ��ݸ���
        void* pUserData) {
        std::cerr <<"errorCounter=" <<errorCounter << "��validation layer: " << pCallbackData->pMessage << std::endl;
        errorCounter++;
        //ͨ��ֻ�ڲ���У��㱾��ʱ�᷵��true����������¾�����VK_FALSE��
        return VK_FALSE;

    }

    //[2]--------------------------------------------------------------------------------------------------------
    //[3]--------------------------------------------------------------------------------------------------------

    /// <summary>
    /// ʹ��glfw�������ڱ���
    /// </summary>
    void createSurface() {
        //Windows�Ĵ�������
            //VkWin32SurfaceCreateInfoKHR createInfo = {};
            //createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            //createInfo.hwnd = glfwGetWin32Window(window);
            //
            ////createInfo.hwnd = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);;
            //createInfo.hinstance = GetModuleHandle(nullptr);
            //if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to create window surface!");
            //}
            //Linux�Ĵ����������������� vkCreateXcbSurfaceKHR
            //ʹ��GLFW����Window surface������������ VkInstance ����GLFW ����ָ�룬�Զ����ڴ���������洢���ص� VkSurfaceKHR ������ڴ��ַ        
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
        if (!enableValidationLayers) return;    //�ó��������ֱ���˳���

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        populateDebugMessengerCreateInfo(createInfo);
        //[4] messenger ������Ϣ�������ȡ�������ĺ�����
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
        //[4]ָ��ϣ�����ûص�����������
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | //�����Ϣ
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |        //�й���Ϊ����Ϣ��һ���Ǵ��󣬵��ܿ�����Ӧ�ó����еĴ���
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;    //�й���Ч��Ϊ����Ϣ�����ܵ��±���
        //[4]���˻ص�֪ͨ����Ϣ���ͣ�������������������
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |    //������һЩ��淶�������޹ص��¼�
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                    //������Υ���淶��һЩ������ʾ�Ĵ��󣬳�����Υ���淶�����������һ�����ܵĴ���
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;                    //�����ŵķ�ʽʹ��Vulkan�������˿���Ӱ��Vulkan���ܵ���Ϊ

        //[4]ָ��ָ��ص�������ָ��
        createInfo.pfnUserCallback = debugCallback;
        //[4]���صĻص�����
        createInfo.pUserData = nullptr;

    }

    //[4]����������
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {//[4]����޷����أ�����������nullptr��
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    //[4]����CreateDebugUtilsMessengerEXT
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
    /// pickPhysicalDevice��������ʹ����ѡ���ͼ���Կ��洢�����ԱVkPhysicalDevice����С�ÿ��GPU�豸��һ��VkPhysicalDevice���͵ľ����ͨ��GPU�豸�ľ�������ǿ��Բ�ѯGPU�豸�����ƣ����ԣ����ܵȡ���VkInstance����ʱ��������󽫻ᱻ��ʽ���٣��������ǲ�����Ҫ��cleanup���������κβ�����
    /// </summary>
    void pickPhysicalDevice() {
        //[5]��ѯGPU����
        uint32_t deviceCount = 0;
        //��һ����vkEnumeratePhysicalDevices�����ҵ�ϵͳ�а�װ��Vulkan�����豸
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);    //��ȡͼ�ο��б�deviceCountΪout���͵ģ��洢�Կ�������
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        else
        {
            std::cout << "�ҵ�֧��Vulkan���Կ���GPU��������" << deviceCount << std::endl;
        }
        //[5]��ȡ������Ϣ����������洢���е�VkPhysicalDevice���
        std::vector<VkPhysicalDevice> devices(deviceCount);
        //�ڶ�����vkEnumeratePhysicalDevices����������devices.data()����pPhysicalDevices,��ɻ��֧�ֵ�[�����豸]���б�
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        //[5]ѡ���ʺϸó����GPU
        int i = 0;
        for (const auto& device : devices)
        {  //���Կ�������������ȷ���豸���������Ǵ����ı�������ʾͼ��
            std::cout<<"devices.size()"<< devices.size()<<std::endl;
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                std::cout << "i=" << i << ", True,  devices.data()=" << devices[i] << std::endl;
                break;    //����ҵ����ʵ��Կ������˳���
            }
            i++;
        }
        if (physicalDevice == VK_NULL_HANDLE) { //������ж���������devices.size()==0Ҳ�ǿ��Ե�
            //δ����ֵ��ֱ���˳�
            throw std::runtime_error("failed to find a suitable GPU!");
        }
        //or
        //[5]ʹ������Map,ͨ�������Զ����Կ�����multimap���������ļ�/ֵ�ԣ��������Ա����ظ���Ԫ��
        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : devices) {

            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
            std::cout << "���Կ��÷֣�score=" << score << std::endl;
        }

        //[5]Check if the best candidate is suitable at all�������Ѻ�ѡ�ģ������豸���Ƿ����
        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        }
        else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }


    //---------------[5]---------------    
    /// <summary>
    /// �����Կ���GPU���豸�Ƿ��ʺϸó����Ҫ��
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    bool isDeviceSuitable(VkPhysicalDevice device) {
        //[5]��ѯ�Կ����ԣ����������ƣ�֧��Vulkan�İ汾��
        VkPhysicalDeviceProperties deviceProperties;
        //��һ������deviceΪ������������������豸���ڶ�������devicePropertiesΪ�������������������
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "�������棺 isDeviceSuitable�� deviceProperties.deviceName=" << deviceProperties.deviceName << "��deviceProperties.limits.maxComputeSharedMemorySize=" << deviceProperties.limits.maxComputeSharedMemorySize << std::endl;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            std::cout << "���Կ�Ϊ�����Կ�" << std::endl;
        else
        {
            std::cout << "���Կ�Ϊ�����Կ�" << std::endl;
        }
        //[5]��չ֧��
        bool extensionsSupported = checkDeviceExtensionSupport(device);
        
        //[5]swap chain support
        //��⽻�����������Ƿ���������,����ֻ������֤��������չ���ú��ѯ��������ϸ����Ϣ
        //����ֻ��Ҫ����������֧��һ��ͼ���ʽ��һ��֧�����ǵĴ��ڱ���ĳ���ģʽ
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            std::cout << "isDeviceSuitable  extensionsSupported=" << extensionsSupported << std::endl;
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        //[5]��ѯ�Կ����ԣ�����������ѹ����64λ�����������ӿ���Ⱦ��VR�ǳ����ã�
        //VkPhysicalDeviceFeatures deviceFeatures;
        //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //[5]�Ƿ�Ϊרҵ�Կ���a dedicated graphics card �������ԣ������Ϊ��ɢ�Կ�,�Ƿ�֧�ּ�����ɫ��
        //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        //or

        QueueFamilyIndices indices = findQueueFamilies(device);    //��⽻�����Ƿ�֧��
        //return indices.graphicsFamily.has_value();    //��ֵ����true
        std::cout << "isDeviceSuitable indices.graphicsFamily.has_value()=" << indices.graphicsFamily.has_value() << std::endl;
        //or
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }


    /// <summary>
    /// ��ѯ������֧��ϸ��
    /// ֻ��齻�����Ƿ���û����������������������ǵĴ��ڱ��治���ݡ� ������������Ҫ���е�����Ҫ�� Vulkan ʵ�����豸������ö࣬�ڽ��н���������֮ǰ��Ҫ���ǲ�ѯ�������Ϣ.����������������ԣ���Ҫ���Ǽ�飺
    ///    1.������������(����������С / ���ͼ����������С / ���ͼ���ȡ��߶�)
    ///    2.�����ʽ(���ظ�ʽ����ɫ�ռ�)
    ///    3.���õĳ���ģʽ
    ///    �뽻������Ϣ��ѯ�йصĺ�������ҪVkPhysicalDevice ����� VkSurfaceKHR��Ϊ����, �����ǽ������ĺ������        
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        //[5]basic surface capabilities ��������
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);//��ѯ������������
        //[5]the supported surface formats
        
        //��ѯ����֧�ֵĸ�ʽ,ȷ�������Ŀռ������������и�ʽ�ṹ��
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

        }
        

        //[5]the supported presentation modes����ѯ֧�ֵĳ���ģʽ
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;

    }
    //Vulkan���еĲ������ӻ��Ƶ�����������Ҫ������ָ���ύ��һ�����к����ִ�У����ҳ�������������Ķ����塣Vulkan �ж��ֲ�ͬ���͵Ķ��У��������ڲ�ͬ�Ķ����壬ÿ��������Ķ���ֻ����ִ���ض���һ����ָ��
    
    /// <summary>
    /// ��ȡ��ͼ�������Ķ����壬����豸֧�ֵĶ�����,���ҳ�������������Ķ�����,��һ�����᷵����������ö��������������������һ�������ͬ���ܵ��ܹ��������еĶ��С��������������ÿ����������Լ�����ÿ����Ķ��е��������������豸������
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        //����豸֧�ֵĶ����壬�Լ���������Щ֧��������Ҫʹ�õ�ָ��
        //[5]Logic to find graphics queue family
        QueueFamilyIndices indices;
        //[5]Logic to find queue family indices to populate struct with
        //[5]C++ 17������optional���ݽṹ�����ִ��ڻ򲻴��ڵ�ֵ�������
        std::optional<uint32_t> graphicsFamily;
        std::cout << "000 findQueueFamilies graphicsFamily.has_value() = " << std::boolalpha << graphicsFamily.has_value() << std::endl; // false
        graphicsFamily = 0;        //��Ҫ�Ķ�����ֻ��Ҫ֧��ͼ��ָ���
        std::cout << "111 findQueueFamilies graphicsFamily.has_value() = " << std::boolalpha << graphicsFamily.has_value() << std::endl; // true

        uint32_t queueFamilyCount = 0;
        //��һ�ε��û�ȡ�豸�Ķ����������Ȼ���������洢������� VkQueueFamilyProperties ���󣬵�һ������ΪIn���͵������豸���ڶ���ΪOut���͵ģ����������豸������ĸ���
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);    //��ȡ�豸�Ķ��������
        std::cout << endl<<"�������豸���Կ����Ķ����������queueFamilyCount=" << queueFamilyCount << std::endl;
        
        //vkQueueFamilyProperties����������ĺܶ���Ϣ�� ����֧�ֵĲ������ͣ��ö�������Դ����Ķ��и���
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        //VkQueueFamilyProperties�ṹ����queueFlags�Ĳ������ͼ���https://blog.csdn.net/qq_36584063/article/details/71219188?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1.topblog&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1.topblog&utm_relevant_index=2

        //�ڶ��ε��ã����������豸�Ķ������б������
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        //������Ҫ�ҵ�����һ��֧��VK_QUEUE_GRAPHICS_BIT���壬��ϵ���еĶ���֧��ͼ�β�����������Ƶ㣬�ߺ������Ρ� 
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            //[5]Ѱ��һ�������壬���ܹ�����window�����Ҵ��г���ͼ�񵽴��ڱ��������Ķ�����
            VkBool32 presentSupport = false;
            //��������豸�Ƿ���г������������Ҵ��г���ͼ�񵽴��ڱ��������Ķ�����
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {    //���ݶ������еĶ����������Ƿ�֧�ֱ���ȷ��ʹ�õı��ֶ����������
                indices.presentFamily = i; 
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                //�ҵ�һ��֧��VK_QUEUE_GRAPHICS_BIT�Ķ����壬VK_QUEUE_GRAPHICS_BIT��ʾ֧��ͼ��ָ��
                indices.graphicsFamily = i;
                
                std::cout << "i=" << i <<"�� queueFamily.queueCount="<<queueFamily.queueCount<<" ��" << std::endl;
                //˵������ʹ����ָ�������ͳ��ֶ�������ͬһ�������壬 ����Ҳ���������ǲ�ͬ�Ķ��������Դ��� ��ʽ��ָ�����ƺͳ��ֶ�������ͬһ���������豸��������ܱ��֡�
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
    /// ����Ƿ�֧�ֽ�������ʵ���ϣ�����豸֧�ֳ��ֶ��У���ô����һ��֧�ֽ���������������û�����ʽ�ؽ��н�������չ�ļ�⣬Ȼ����ʽ�����ý�������չ��
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);    //ö���豸��չ�б�����������չ�Ƿ���ڣ����õ���չ������
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);        //������չ����
        
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        int i = 0;
        for (const auto& extension : availableExtensions) {
            std::cout << "i=" << i << "��checkDeviceExtensionSupport extension.extensionName=" << extension.extensionName << std::endl;
            requiredExtensions.erase(extension.extensionName);
            i++;
        }
        bool empty = requiredExtensions.empty();
        std::cout << "requiredExtensions.empty()=" << empty << std::endl;
        return requiredExtensions.empty();
    }

    
    /// <summary>
    /// ���Կ����֣��ڶ���Կ������ʵ�����£���ÿһ���豸Ȩ��ֵ��ѡ����ߵ�һ��
    /// </summary>
    /// <param name="device"></param>
    /// <returns></returns>
    int rateDeviceSuitability(VkPhysicalDevice device) {
        //[5]��ѯ�Կ����ԣ�������ID�����͡����ƣ�֧��Vulkan�İ汾��
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);    //�����Կ��������豸�����õ���Ӧ����
        std::cout << std::endl
            << "deviceProperties.deviceID=" << deviceProperties.deviceID
            << " deviceProperties.vendorID=" << deviceProperties.vendorID
            << " deviceProperties.deviceType=" << deviceProperties.deviceType
            << "  deviceProperties.deviceName=" << deviceProperties.deviceName    //�豸���ƣ��ҵ���GeForce GTX 1660 Ti��AMD Radeon(TM) Graphics
            << "  deviceProperties.driverVersion=" << deviceProperties.driverVersion    //�������ڿ����豸����������İ汾
            << "  deviceProperties.apiVersion=" << deviceProperties.apiVersion    //�豸֧�ֵ���߰汾��Vulkan
            << std::endl;

        int score = 0;
        //[5]��ɢGPU������������������
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
            std::cout << "����ɢGPU����ֵ����1000" << std::endl;
        }
        else
        {
            std::cout << "����ɢGPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU���м���GPU���� deviceProperties.deviceType=" << deviceProperties.deviceType << std::endl;
        }
        //[5]֧����������ֵ��Ӱ��ͼ������
        score += deviceProperties.limits.maxImageDimension2D;

        printf("֧�ֵ����1D����ֵ��deviceProperties.limits.maxImageDimension1D=%d\n", deviceProperties.limits.maxImageDimension1D);
        printf("֧�ֵ����2D����ֵ��deviceProperties.limits.maxImageDimension2D=%d\n", deviceProperties.limits.maxImageDimension2D);
        printf("֧�ֵ����3D����ֵ��deviceProperties.limits.maxImageDimension3D=%d\n", deviceProperties.limits.maxImageDimension3D);

        printf("֧�ֵ����DimensionCube��deviceProperties.limits.maxImageDimensionCube=%d\n", deviceProperties.limits.maxImageDimensionCube);

        //[5]��ѯ�Կ����ԣ�����������ѹ����64λ�����������ӿ���Ⱦ��VR��
        VkPhysicalDeviceFeatures deviceFeatures;
        //��һ������ΪIn���͵ģ����������豸���ڶ�������ΪOut���͵ģ������豸�ص�Ľṹ��
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        std::cout << "deviceFeatures.multiViewport=" << deviceFeatures.multiViewport
            << " deviceFeatures.textureCompressionETC2=" << deviceFeatures.textureCompressionETC2
            << " deviceFeatures.textureCompressionBC=" << deviceFeatures.textureCompressionBC
            << std::endl;
        //[5]��֧�ּ�����ɫ��
        if (!deviceFeatures.geometryShader) {
            score = 0;
            //return 0;
        }
        else
        {
            std::cout << "֧�ּ�����ɫ�� score=" << score << std::endl;
        }
        if (deviceFeatures.tessellationShader)
        {
            std::cout << "֧������ϸ����ɫ����tessellationShader" << std::endl;
        }
        return score;

    }
    

    /// <summary>
    /// �����߼��豸����Ϊ�����豸�Ľ����ӿڣ��߼��豸������������ʹ�õ������豸�����ԡ��߼��豸����������instance�����������ƣ�Ҳ��Ҫ����������Ҫʹ�õĹ��ܡ���Ϊ�����Ѿ���ѯ����Щ���дؿ��ã���������Ҫ��һ��Ϊ�߼��豸�����������͵�������С�����в�ͬ������Ҳ���Ի���ͬһ�������豸��������߼��豸��
    /// </summary>
    void createLogicalDevice() {
        //��ȡ����ͼ�������Ķ�����
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        VkDeviceQueueCreateInfo queueCreateInfo = {};    //�����߼��豸��Ҫ��д�ṹ�壬�����������һ����������������Ķ�������
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        //Ŀǰ���ԣ�����ÿ�������壬��������ֻ���������������Ķ��У���ʵ���ϣ�����ÿһ�������壬���Ǻ�����Ҫһ�����ϵĶ��С����ǿ����ڶ���̴߳���ָ��壬Ȼ�������߳�һ�ν�����ȫ���ύ�����͵��ÿ����� Vulkan��Ҫ���Ǹ������һ��0.0��1.0֮��ĸ�������Ϊ���ȼ�������ָ����ִ��˳��
        queueCreateInfo.queueCount = 1;
        //[6]Vulkanʹ��0.0��1.0֮��ĸ�����Ϊ���з������ȼ�, �����л�����ִ�еĵ��ȡ���ʹֻ��һ�����У���Ҳ�Ǳ���ģ���ʽ�ظ���������ȼ���
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        //[6]device features��ָ��Ӧ�ó���ʹ�õ��豸����
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};        //��д�߼��豸�Ľṹ��
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;    //pQueueCreateInfos ָ��ָ�� queueCreateInfo �ĵ�ַ
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;    //pEnabledFeatures ָ��ָ�� deviceFeatures �ĵ�ַ
        
        //[6]VK_KHR_swapchain �����豸����Ⱦͼ����ʾ��windows
        //[6]֮ǰ�汾Vulkanʵ�ֶ�ʵ�����豸�ض�����֤����������֣����������������ˡ�
        //[6]����ζ��VkDeviceCreateInfo��enabledLayerCount��ppEnabledLayerNames�ֶα����µ�ʵ�ֺ��ԡ�����������Ӧ�ý���������Ϊ��Ͼɵ�ʵ�ּ��ݣ�
        createInfo.enabledExtensionCount = 0;
        if (enableValidationLayers) {
            //�Զ��豸�� Vulkan ʵ��ʹ����ͬ��У���
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            std::cout << "createLogicalDevice()��������enableValidationLayers" << std::endl;
        }
        else {//�����˸÷�֧
            createInfo.enabledLayerCount = 0;
            std::cout << "createLogicalDevice()��û������enableValidationLayers" << std::endl;
        }

        //[6]create a queue from both families
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            //���������һ����������������Ķ�����������д[�߼��豸]�Ķ��еĹ�����Ϣ
            VkDeviceQueueCreateInfo queueCreateInfo = {};    //��������ʹ�õ��Ķ�����
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pNext = nullptr;    //����ӵ�
            queueCreateInfo.queueFamilyIndex = queueFamily;        //��ͼ�������Ķ���
            queueCreateInfo.queueCount = 1;        //����ʹ�ö��е�����
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);  //push_back�Ḻ��һ��ֵ����vector�����βԪ�ء�ѹ����push����vector����ġ�β��(back)��

        }
        //[6]��������Ϣ��������info
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        //[6]������չ֧�֣����ý�����
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());    
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        //[6]�����߼��豸������physicalDevice, &createInfo, nullptrΪin���͵ģ�deviceΪOut���͵ģ������߼��豸
        /**        vkCreateDevice �����Ĳ���:
        1.�������߼��豸���н����������豸����
        2.ָ������Ҫʹ�õĶ�����Ϣ
        3.��ѡ�ķ������ص���������Ϊnullptr,��ʹ��vulkan���õ��ڴ������
        4.�����洢���ص��߼��豸������ڴ��ַ
        �߼��豸����ֱ����Vulkanʵ������,���Դ����߼��豸ʱ����Ҫʹ��Vulkanʵ����Ϊ����          */

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        //[6]��ȡ��������
        //��ȡָ��������Ķ��о��
        //���Ĳ����������߼��豸����,����������,��������,�����洢���صĶ��о�����ڴ��ַ
        //��Ϊ����ֻ��������д���һ�����У���������ֻʹ������0
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        //[6]��ʾ����
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }
    //[6]----------------------------------------------------------------------------

    /// <summary>
    /// ����������ȾĿ��ļ��ϣ�Vulkan û��Ĭ��֡����ĸ������Ҫһ���ܹ�������Ⱦ������������� Vulkan �У���һ������ǽ�������Vulkan �Ľ�����������ʽ�ش�����������Ĭ�ϵĽ���������������������һ�����������ɵȴ����ֵ�ͼ����С����ǵ�Ӧ�ó���ӽ�������ȡһ��ͼ��Ȼ����ͼ���Ͻ�����Ⱦ��������ɺ󣬽�ͼ�񷵻ص��������Ķ����С��������Ķ��еĹ�����ʽ��������ͼ�񵽱�������������ڽ����������á���ͨ����˵��������������ͬ��ͼ����ֺ���Ļˢ��
    /// </summary>
    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        //[7]�򵥵ؼ�������Сֵ��ζ��������ʱ������Ҫ�ȴ�������������ڲ�������
        //[7]Ȼ����ܻ�ȡ��һ��Ҫ��Ⱦ��ͼ����ˣ������������ٱ���Сֵ��һ��ͼ��
        //[7]uint32_t imageCount = swapChainSupport.capabilities.minImageCount;
         
        /**     ���ð����������е�ͼ�������Ҳ���ǽ������Ķ��п������ɵ�ͼ������� ����ʹ�ý�����֧�ֵ���Сͼ����� +1 ������ͼ����ʵ���������塣maxImageCount ��ֵΪ 0 ������ֻҪ�ڴ�������㣬���ǿ���ʹ������������ͼ��
          */

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        //��������������Ҫ�Ľṹ��
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        //[7]imageArrayLayersָ��ÿ��ͼ������Ĳ�����������������ڿ���3DӦ�ó��򣬷����ֵʼ��Ϊ1��
        //ָ��ÿ��ͼ���������Ĳ��.������ VR ��ص�Ӧ�ó�����˵����ʹ�ø���Ĳ��
        createInfo.imageArrayLayers = 1;
        //ָ�����ǽ���ͼ���Ͻ��������Ĳ���--��������Ϊ�����Ŀ��ͼ��
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        //[7]���ַ������Դ���Ӷ�����з��ʵ�ͼ��
        //[7]VK_SHARING_MODE_CONCURRENT
        //[7]VK_SHARING_MODE_EXCLUSIVE
        cout << "in createSwapChain()" << endl;
        /**
       ָ���ڶ��������ʹ�ý�����ͼ��ķ�ʽ����һ���ö���ͼ�ζ��кͳ��ֶ��в���ͬһ�����е�������źܴ�Ӱ�졣
       ����ͨ��ͼ�ζ����ڽ�����ͼ���Ͻ��л��Ʋ�����Ȼ��ͼ���ύ�����ֶ�������ʾ�������ֿ����ڶ�����з���ͼ��ķ�ʽ:
       VK_SHARING_MODE_EXCLUSIVE��һ��ͼ��ͬһʱ��ֻ�ܱ�һ����������ӵ�У� ����һ������ʹ����֮ǰ��������ʽ�ظı�ͼ������Ȩ����һģʽ�����ܱ�����ѡ�
       VK_SHARING_MODE_CONCURRENT��ͼ������ڶ���������ʹ�ã�����Ҫ��ʽ�ظı�ͼ������Ȩ��

       ���ͼ�κͳ��ֲ���ͬһ�������壬����ʹ��Эͬģʽ�����⴦��ͼ������Ȩ���⡣Эͬģʽ��Ҫ����ʹ�� queueFamilyIndexCount ��pQueueFamilyIndices ��ָ����������Ȩ�Ķ����塣
       ���ͼ�ζ�����ͳ��ֶ�������ͬһ�������� (�󲿷�����¶�������)�� ���ǾͲ���ʹ��Эͬģʽ��Эͬģʽ��Ҫ����ָ������������ͬ�Ķ����塣
       */
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        std::cout << "---------indices.graphicsFamily=" << indices.graphicsFamily.value() << "��indices.presentFamily=" << indices.presentFamily.value()<<", indices.graphicsFamily.has_value="<< indices.graphicsFamily.has_value() << ", indices.graphicsFamily.has_value()="<< indices.presentFamily.has_value() << std::endl;
        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;            
        }
        else {//����÷�֧
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional            
        }
        
        //[7]ָ���Խ������е�ͼ��Ӧ��ĳ�ֱ任�����ǿ���Ϊ�������е�ͼ��ָ��һ���̶��ı任����(��Ҫ����������supportedTransforms ����)������˳ʱ����ת 90 �Ȼ���ˮƽ��ת���������Ҫ�����κα任������ָ��ʹ�� currentTransform �任���ɡ�
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        //[7]alpha channel should be used for blending
        //��Ա��������ָ�� alpha ͨ���Ƿ������ʹ���ϵͳ�е��������ڽ��л�ϲ����� ͨ�������ǽ�������Ϊ VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR�����Ե�alphaͨ����
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;        //���ó���ģʽ
        createInfo.clipped = VK_TRUE;        //����ΪTrueʱ����ʾ���ǲ����ı�����ϵͳ�е����������ڵ������ص���ɫ
        //[7]����������ȡ������ͼ��ʽ��oldSwapchain��Ҫָ����������ΪӦ�ó��������й����н��������ܻ�ʧЧ�� ���磬�ı䴰�ڴ�С�󣬽�������Ҫ�ؽ����ؽ�ʱ��Ҫ֮ǰ�Ľ�����
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        //����������
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        /** �����ڴ���������ʱָ����һ��minImageCount��Ա������������С��Ҫ�Ľ�����ͼ�������� Vulkan �ľ���ʵ�ֿ��ܻᴴ���������С������ͼ����������Ľ�����ͼ�����������������Ȼ��Ҫ��ʽ�ز�ѯ������ͼ��������ȷ��������� */
        //��ȡ������ͼ����
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        //�洢�������õĽ�����ͼ���ʽ�ͷ�Χ
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    //[7]��ѡ���ʵ��ı����ʽ
     /** ÿһ�� VkSurfaceFormatKHR ��Ŀ������һ�� format �� colorSpace��Ա����
     *format ��Ա��������ָ����ɫͨ���ʹ洢����,��VK_FORMAT_B8G8R8A8_UNORM ��ʾ������B��G��R �� A ��˳��    ÿ����ɫͨ���� 8 λ�޷�����������ʾ,�ܹ�ÿ����ʹ�� 32 λ��ʾ
         * colorSpace ��Ա����������ʾ SRGB ��ɫ�ռ��Ƿ�֧�֣��Ƿ�ʹ�� VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ��־��������ɫ�ռ�,���SRGB��֧��,���Ǿ�ʹ��SRGB,�����Եõ�����׼ȷ����ɫ��ʾ
         * ����ʹ��VK_FORMAT_B8G8R8A8_UNORM��ʾRGB ��Ϊ��ɫ��ʽ
         */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            //����ʽ�б����Ƿ���������Ҫ�趨�ĸ�ʽ�Ƿ����
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        //[12]�����ѯʧ�ܷ��ص�һ��������������б����ҵ�������Ҫ�ĸ�ʽ�����ǿ��Զ��б��д��ڵĸ�ʽ���д�֣�ѡ�������ߵ��Ǹ���Ϊ����ʹ�õĸ�ʽ����Ȼ�����������£�ֱ��ʹ���б��еĵ�һ����ʽҲ�Ƿǳ������ѡ��
        return availableFormats[0];
    }
    
    
    /// <summary>
    /// ������ѵĿ��ó���ģʽ������ģʽ�ǽ�����������Ҫ�����ã���������ʲô������ͼ��Ż���ʾ����Ļ��Vulkan�ṩ��4�ֿ��õĳ���ģʽ��
    /// [7]VK_PRESENT_MODE_IMMEDIATE_KHR��Ӧ�ó����ύ��ͼ��ᱻ�������䵽��Ļ�ϣ����ܻᵼ��˺������
    //  [7]VK_PRESENT_MODE_FIFO_KHR����֤һ�����ý��������һ���Ƚ��ȳ��Ķ��У�ÿ�δӶ���ͷ��ȡ��һ��ͼ�������ʾ��Ӧ�ó�����Ⱦ��ͼ���ύ���������󣬻ᱻ���ڶ���β����������Ϊ��ʱ��Ӧ�ó�����Ҫ���еȴ�����һģʽ�ǳ��������ڳ��õĴ�ֱͬ����ˢ����ʾ��ʱ��Ҳ��������ֱ��ɨ��
    //  [7]VK_PRESENT_MODE_FIFO_RELAXED_KHR ��һģʽ����һģʽ��Ψһ�����ǣ����Ӧ�ó����ӳ٣����½������Ķ�������һ�δ�ֱ��ɨʱΪ�գ� ��ô�����Ӧ�ó�������һ�δ�ֱ��ɨǰ�ύͼ��ͼ�����������ʾ����һģʽ���ܻᵼ��˺������
    //  [7]VK_PRESENT_MODE_MAILBOX_KHR��������ѣ���һģʽ�ǵڶ���ģʽ����һ�����֡��������ڽ������Ķ�����ʱ����Ӧ�ó��򣬶����е�ͼ��ᱻֱ���滻ΪӦ�ó������ύ��ͼ����һģʽ��������ʵ���������壬����˺�������ͬʱ��С���ӳ�����
    /// </summary>
    /// <param name="availablePresentModes"></param>
    /// <returns></returns>
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        //[7]����������ã�����оͿ���
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }


    /// <summary>
    /// ��minImageExtent��maxImageExtent��ѡ���봰����ƥ��ķֱ��ʣ����ý�����Χ��
    /// ������Χ�ǽ�������ͼ��ķֱ��ʣ����������Ǻ�����Ҫ��ʾͼ��Ĵ��ڵķֱ�����ͬ�� VkSurfaceCapabilitiesKHR �ṹ�嶨���˿��õķֱ��ʷ�Χ�� Vulkan ͨ�� currentExtent ��Ա��������֪�ʺ����Ǵ��ڵĽ�����Χ�� һЩ����ϵͳ��ʹ��һ������ֵ��uint32_t �������͵����ֵ�� ��ʾ���������Լ�ѡ����ڴ�������ʵĽ�����Χ��������ѡ��Ľ�����Χ��Ҫ�� minImageExtent �� maxImageExtent �ķ�Χ�ڡ�
    /// ������ max �� min ��������������ķ�Χ��ѡ�񽻻���Χ�ĸ߶�ֵ�Ϳ��ֵ����Ҫ��Դ�ļ��а��� algorithm ͷ�ļ����ܹ�ʹ������
    /// </summary>
    /// <param name="capabilities"></param>
    /// <returns></returns>
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            //[16]Ϊ����ȷ�ش����ڴ�С����������Ҫ��ѯ֡�������ĵ�ǰ���ڴ�С����ȷ����swap chain(new)��ͼ���С��ȷ��
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

    //[8]--------ImageView��ͼ����ͼ����ָ��ͼ��Ĵ�С---------------------------------------

    void createImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());        //�����㹻������ռ����洢ͼ����ͼ
        for (size_t i = 0; i < swapChainImages.size(); i++)        //�������н�����ͼ�񣬴���ͼ����ͼ
        {//����ͼ����ͼ�ṹ�������Ϣ
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            //[8]ѡ����ͼ���� 1D 2D 3D
            //viewType �� format ��Ա��������ָ��ͼ�����ݵĽ��ͷ�ʽ��
            //viewType����ָ��ͼ�񱻿�����һά������ά������ά��������������ͼ
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            //[8]components�ֶ�������ת��ɫͨ���� 
            //components ��Ա�������ڽ���ͼ����ɫͨ����ӳ�䡣���磬���ڵ�ɫ�������ǿ��Խ�������ɫͨ��ӳ�䵽��ɫͨ��������Ҳ����ֱ�ӽ���ɫͨ����ֵӳ��Ϊ���� 0 �� 1��������, ����ֻʹ��Ĭ�ϵ�ӳ��
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; //default
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            //[8]����ͼ�����;�Լ�Ӧ����ͼ����ĸ����֡�
            //subresourceRange����ָ��ͼ�����;��ͼ�����һ���ֿ��Ա�����
            //��������ǵ�ͼ��������ȾĿ�꣬����û��ϸ�ּ���ֻ����һ��ͼ��
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            //��������ڱ�д VR һ���Ӧ�ó��򣬿��ܻ�ʹ��֧�ֶ����εĽ���������ʱ������Ӧ��Ϊÿ��ͼ�񴴽����ͼ����ͼ���ֱ������������ۺ�����������ͬ��ͼ��
            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
            {//����ͼ����ͼ������ͼ����ͼ���Ϳ��Խ�ͼ����Ϊ����ʹ�ã�����Ϊ��ȾĿ�꣬����Ҫ֡�������
                throw std::runtime_error("failed to create image views!");
            }

        }
    }
    //[8]--------------------------------------------------------------------------------------------------------

    //[9]--------------------------------------------------------------------------------------------------------
    void createRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        //[9]colorAttachment��formatӦ��swapChainͼ��ĸ�ʽƥ��
        colorAttachment.format = swapChainImageFormat;
        //[9]û��ʹ�ö��ز�����multisampling������ʹ��1��������
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        //[9]clear the framebuffer to black before drawing a new frame
        //[9]VK_ATTACHMENT_LOAD_OP_LOAD ����Attachment����������
        //[9]VK_ATTACHMENT_LOAD_OP_CLEAR ��ʼʱ��ֵ��ʼ��Ϊ����
        //[9]VK_ATTACHMENT_LOAD_OP_DONT_CARE ��������δ���壻 ����
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //[9]color and depth data
        //[9]VK_ATTACHMENT_STORE_OP_STORE ��Ⱦ�����ݽ��洢���ڴ��У��Ժ���Զ�ȡ
        //[9]VK_ATTACHMENT_STORE_OP_DONT_CARE ��Ⱦ������֡�����������ݽ���δ�����
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //[9]stencil data
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //[9]Textures and framebuffers 
        //[9]ָ������Ⱦ��ʼ֮ǰͼ�񽫾��еĲ��֡�
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //[9]ָ������Ⱦ���ʱ�Զ����ɵ��Ĳ��֡�
        //[9]VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL used as color attachment
        //[9]VK_IMAGE_LAYOUT_PRESENT_SRC_KHR Images the swap chain ��Ҫ��ʾ��ͼ��
        //[9]VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL  �����洢�����Ʋ���Ŀ��ͼ��
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //[9]����Attachment
        VkAttachmentReference colorAttachmentRef = {};
        //[9]attachment����ͨ��attachment���������е�����ָ��Ҫ���õ�attachment
        colorAttachmentRef.attachment = 0;
        //[9]����ָ��������ϣ��attachment��ʹ�ô����õ�subpass�о������ֲ��֡�
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //[9]Vulkan may also support compute subpasses in the future
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        //[9]����colorAttachment
        //[9]������attachment��������ֱ�Ӵ�fragment shader ���õģ�location = 0��out vec4 outColorָ�
        //[9]subpass�������������������͵�attachment
        //[9]pInputAttachments read from a shader
        //[9]pResolveAttachments used for multisampling attachments
        //[9]pDepthStencilAttachment depth and stencil data
        //[9]pPreserveAttachments not used by this subpass but for which the data must be preserved(����)
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
    /// 10 ͼ�ι��ߣ�����ͼ���Կ������״̬�������ӿڴ�С����Ȼ����������ɫ��
    /////ͼ�ι�����һϵ�н������ύ�Ķ��������ת��Ϊ��ȾĿ���ϵ����صĲ��������ļ򻯹������£�
    //    input assembler:��ȡ�������ݣ��������ݵ���Դ������Ӧ�ó����ύ��ԭʼ�������ݣ����������������ȡ�Ķ������ݡ�
    //    vertex shader ��ÿ���������ģ�Ϳռ䵽��Ļ�ռ�ı任��Ȼ�󽫶������ݴ��ݸ�ͼ�ι��ߵ���һ�׶Ρ�
    //    tessellation shaders ����һ���Ĺ���Լ���ͼ�ν���ϸ�֣��Ӷ��������������ͨ��������ʹ����ǽ�����಻�⻬���濴��������Ȼ��
    //    geometry shader ������ͼԪ(�����Σ��߶Σ���) Ϊ��λ������ͼ�Σ��������޳�ͼԪ�����ͼԪ���е������� tessellation shader����������Ŀǰ�Ѿ����Ƽ�Ӧ�ó���ʹ������geometry shader �������ڳ���Intel �����Կ���Ĵ�����Կ��ϱ��ֲ��ѡ�
    //    rasterization �׶ν�ͼԪ��ɢΪƬ�Ρ�Ƭ�α�������֡������������ء�λ����Ļ���Ƭ�λᱻ������������ɫ������Ķ������Ի���Ƭ��֮����в�ֵ��������Ȳ��Ժ�λ������Ƭ��֮���Ƭ��Ҳ�ᱻ������
    //    fragment shader ��ÿһ��δ��������Ƭ�ν��д���ȷ��Ƭ��Ҫд���֡���壬������ʹ������ vertex shader �Ĳ�ֵ���ݣ�������������Ͷ��㷨�ߡ�
    //    color blending �׶ζ�д��֡����ͬһ����λ�õĲ�ͬƬ�ν��л�ϲ�����Ƭ�ο���ֱ�Ӹ���֮ǰд���Ƭ�Σ�Ҳ���Ի���֮ǰƬ��д�����Ϣ���л�ϲ�����
    //    ԭ�����ӣ�https ://blog.csdn.net/yuxing55555/article/details/88976664
    /// </summary>
    void createGraphicsPipeline()
    {
        auto vertShaderCode = readFile("vert.spv");
        auto fragShaderCode = readFile("frag.spv");
        //[10]
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
         
        //[10]���Խ������ɫ����ϵ�һ��ShaderModule�У���ʹ�ò�ͬ��entry points���������ǵ���Ϊ��
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        //[10]ָ����ɫ��������ֵ�� 
        //[10]������ɫ��ģ�飬�ڹܵ��д������������費ͬ��ֵ����������Ϊ��
        //[10]������Ⱦʱʹ�ñ���������ɫ����Ϊ��Ч�� ���������Խ����Ż�������������������Щֵ��if���
        //[10]���û�������ĳ���������Խ���Ա����Ϊnullptr����ʼ�����Զ�ִ�иò�����
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
          ����������Ҫ���������������棺
        �󶨣�����֮��ļ��������ǰ��𶥵�ķ�ʽ���ǰ���ʵ���ķ�ʽ������֯
        �������������ݸ�������ɫ�����������ͣ����ڽ����԰󶨵�������ɫ���еı���
        ��������ֱ���ڶ�����ɫ����Ӳ���붥�����ݣ����ﲻ�����κζ�������
          */

        //[10]//�������ݸ�������ɫ���Ķ������ݸ�ʽ--��������
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        //[10]�����������ڼ��ض������ݵ���ϸ��Ϣ
        //����ָ����������������֯��Ϣ�ؽṹ������
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        //[10]�����������ڼ��ض������ݵ���ϸ��Ϣ
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
        //[10]minDepth��maxDepth ��0.0f��1.0f֮��
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        //[10]�ü����ζ�����Щ�������ر��洢
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;

        //[10]viewport ��scissor�����ж��
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        //[10]������ƽ���Զƽ��֮���ƬԪ
        rasterizer.depthClampEnable = VK_FALSE;
        //[10]true ����ͼ�β���ͨ����դ���׶Ρ� ���ö�֡���������κ����
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        //[10]VK_POLYGON_MODE_FILL ��Ƭ�������������
        //[10]VK_POLYGON_MODE_LINE ����α�Ե����Ϊ��
        //[10]VK_POLYGON_MODE_POINT ����ζ������Ϊ��
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        //[10]֧�ֵ�����߿�ȡ����Ӳ�����κα�1.0f�ֵ��߶���Ҫ����wideLines��
        rasterizer.lineWidth = 1.0f;
        //[10]ȷ��Ҫʹ�õ��沿�޳����͡�
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        //[10]ָ����Ƭ��Ϊ����Ķ���˳�򣬿�����˳ʱ�����ʱ��
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        //[10]Rasterization����ͨ����ӳ��������Ƭ�ε�б�ʶ����ֵ����ƫ�����������ֵ��
        //[10]��������Ӱ��ͼ���粻��Ҫ��depthBiasEnable����ΪVK_FALSE��
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

        //[10]specification���˵���ĵ�
        //[10]ÿ�����ӵ�֡�������Ļ�Ϲ��� 
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        //[6]Ƭ����ɫ������ɫֱ�����
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
        //[10]͸�����
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

        //[10]ȫ����ɫ������á�
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        //[10]bitwise combination ��ע�⣬�⽫�Զ����õ�һ�ַ���
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
        //[10]���ý�ʹ��ͼ�ι��ߵ�renderPass��subpass������
        //[10]Ҳ����ʹ��������Ⱦ���ߣ��������Ǳ�����renderPass����
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        //[10]Vulkan����ͨ�������й����������������µ�ͼ�ι��ߡ� 
        //[10]����������˼���ǣ������߾��������й��߹��е���๦��ʱ���������ߵĳɱ��ϵͣ�
        //[10]���ҿ��Ը�������ͬһ������֮����л��� 
        //[10]����ʹ��basePipelineHandleָ�����й��ߵľ����Ҳ����ʹ��basePipelineIndex����������������һ�����ߡ�
        //[10]����ֻ��һ�����ߣ��������ֻ��ָ��һ���վ����һ����Ч������
        //[10]������VkGraphicsPipelineCreateInfo��flags�ֶ��л�ָ����VK_PIPELINE_CREATE_DERIVATIVE_BIT��־ʱ����ʹ����Щֵ��
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional


        //[10]�ڶ�������VK_NULL_HANDLE������һ����ѡ��VkPipelineCache����
        //[10]����Cache�����ڴ洢�����ù��ߴ�����ص�����
        //[10]���Լӿ���ߵĴ����ٶ�,�������
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }


        //[10]
        //�ڴ���ͼ�ι���֮ǰ�����ὫSPIR-V�ֽ�����벢���ӵ����������Թ�GPUִ�С�
        //����ζ���ڹܵ�������ɺ󣬾Ϳ�����������ShaderModule
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
    /// ����֡�������֡����������ͼ����ͼ��ָ��ʹ�õ���ɫ����Ⱥ�ģ��
    /// </summary>
    void createFramebuffers()
    {
        //[11]����������С����������֡������
        swapChainFramebuffers.resize(swapChainImageViews.size());    //�����㹻�Ŀռ����洢����֡�������

        //[11] create framebuffers
        for (size_t i = 0; i < swapChainImageViews.size(); i++) 
        { //Ϊ��������ÿһ��ͼ����ͼ���󴴽���Ӧ��֡����
            VkImageView attachments[] = { swapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            //ָ��֡������Ҫ���ݵ���Ⱦ���̶���. ֮�����Ⱦ���������ǿ���ʹ�������ָ������Ⱦ���̶�������ݵ�������Ⱦ���̶���.һ����˵��ʹ����ͬ��������ͬ���͸��ŵ���Ⱦ���̶���������ݵġ�
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;    //ָ�����Ÿ���
            framebufferInfo.pAttachments = attachments; //ָ����Ⱦ���̶�����������������Ϣ�� pAttachment ����
            //ָ��֡����Ĵ�С
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;        //ָ��ͼ�����,����Ľ�����ͼ���ǵ����

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

    }
    //[11]-------------------------------------------------------------------------------

    //[12]-------------------------------------------------------------------------------
    //Vulkan �µ�ָ��������ָ����ڴ洫��ָ�����ֱ��ͨ����������ִ�еġ�������Ҫ������Ҫִ�еĲ�����¼��һ��ָ������Ȼ���ύ������ִ����Щ�����Ķ��в���ִ�С���ʹ�����ǿ����ڳ����ʼ��ʱ��׼��������Ҫָ����ָ�����У�����Ⱦʱֱ���ύִ�С�Ҳʹ�ö��߳��ύָ���ø������ס�����ֻ��Ҫ����Ҫָ��ִ�е�ʹ�ã���ָ�������ύ�� Vulkan ����ӿڡ�
    
    /// <summary>
    /// Command pool ����command buffers������ָ����
    /// ָ�������ڱ��ύ������֮ǰ��ȡ�Ķ��к󣬱� Vulkan ִ�С�ÿ��ָ��ض�������ָ������ֻ���ύ��һ���ض����͵Ķ��С����� �����ʹ�õ��ǻ���ָ������Ա��ύ��֧��ͼ�β����Ķ��С�
    //��������������ָ��ض��󴴽��ı�ǣ� �����ṩ���õ���Ϣ��Vulkan�������������һ���Ż�����
    //VK_COMMAND_POOL_CREATE_TRANSIENT_BIT�� ʹ���������ָ������Ƶ��������¼�µ�ָ�� (ʹ����һ��ǿ��ܻ�ı�֡���������ڴ�������)
    //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT�� ָ������֮���໥���������ᱻһ�����á� ��ʹ����һ��ǣ�ָ������ᱻ����һ������
    /// </summary>
    void createCommandPool() {
        std::cout << "in createCommandPool" << std::endl;
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo = {};    //ָ��صĴ���ֻ��Ҫ2������
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        //[12]There are two possible flags for command pools
        //[12]VK_COMMAND_POOL_CREATE_TRANSIENT_BIT ��ʾCommandPool���������������¼�¼�����ܻ�ı��ڴ������Ϊ��
        //[12]VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ���������¼�¼��CommandPool�����򶼱���һ������
        poolInfo.flags = 0; //��ѡ�Ĳ�ʹ��flag

         //ָ��ض���Ĵ���
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

    }

    //[13]---------------------------------------------------------------------------------------
    
    /// <summary>
    /// Command buffers���ڼ�¼ָ�������������Ⱦ
    /// ָ������������¼����ָ����ڻ��Ʋ�������֡�����Ͻ��еģ�������ҪΪ�������е�ÿһ��ͼ�����һ��ָ������
    /// </summary>
    void createCommandBuffers() {
        commandBuffers.resize(swapChainFramebuffers.size()); //ָ������ʹ�õ�ָ��غ���Ҫ�����ָ���������

        //VkCommandBuffer��Ҫʹ��VkCommandPool�����䡣���ǿ���Ϊÿ���߳�ʹ��һ��������VkCommandPool���������ͬ������ͬVkCommandPoolʹ���Լ����ڴ���Դ����VkCommandBuffer����ʼ��¼VkCommandBuffer�󣬵��õ�GPUָ��ᱻд��VkCommandBuffer���ȴ��ύ������ִ��
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        /**
        level ��Ա��������ָ�������ָ����������Ҫָ����������Ǹ���ָ������:
        VK_COMMAND_BUFFER_LEVEL_PRIMARY�� ���Ա��ύ�����н���ִ�У������ܱ�����ָ��������á�
        VK_COMMAND_BUFFER_LEVEL_SECONDARY�� ����ֱ�ӱ��ύ�����н���ִ�У������Ա���Ҫָ���������ִ��

        ���������û��ʹ�ø���ָ�����󣬵�����������������ĺô����Զ��׼��ģ����ǿ��԰�һЩ���õ�ָ����ڸ���ָ������ Ȼ������Ҫָ�������е���ִ��
          */
        //[13]ָ��primary �� secondary command buffers.
    
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
        //����ָ������
        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        //[13]recording a command buffer //��¼ָ�ָ���
        for (size_t i = 0; i < commandBuffers.size(); i++)
        {//ָ��һЩ�й�ָ����ʹ��ϸ��
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            /**
           flags ��Ա��������ָ�����ǽ�Ҫ����ʹ��ָ��塣����ֵ������������Щ:
           VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT�� ָ�����ִ��һ�κ󣬾ͱ�������¼�µ�ָ��.
           VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT�� ����һ��ֻ��һ����Ⱦ������ʹ�õĸ���ָ���.
           VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT�� ��ָ���ȴ�ִ��ʱ����Ȼ�����ύ��һָ���
             */
            //[13]ָ��ָ�����ǽ����ʹ��command buffers.
            //[13]VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ��ִ��һ�κ�����rerecord��
            //[13]VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT ����һ������command buffers, ���ڵ�����Ⱦͨ����ʹ��
            //[13] VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT command buffer �������Ѿ�����ִ��ʱ�����ύ��
            beginInfo.flags = 0; // Optional
            //[13]����secondary command buffers ��ء� ��ָ����primarycommand buffers �̳���Щ״̬��
            //���ڸ���ָ��壬����������ָ���ӵ���������Ҫָ���̳е�״̬
            beginInfo.pInheritanceInfo = nullptr; // Optional
            //ָ�������¼ָ��󣬵���vkBeginCommandBuffer����������ָ������
            //��ʼָ���ļ�¼����

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            //[13]Starting a render pass//ָ��ʹ�õ���Ⱦ���̶���
            VkRenderPassBeginInfo renderPassInfo = {};
            //[13]the render pass itself and the attachments to bind
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;          //ָ��ʹ�õ���Ⱦ���̶���
            renderPassInfo.framebuffer = swapChainFramebuffers[i];//ָ��ʹ�õ�֡�������
            //[13]the size of the render area
            /**renderAreaָ��������Ⱦ������λ����һ��������������ݻᴦ��δ����״̬��ͨ�������ǽ���һ��������Ϊ������ʹ�õĸ��Ŵ�С��ȫһ��.
              */
            renderPassInfo.renderArea.offset = { 0, 0 }; //It should match the size of the attachments for best performance
            renderPassInfo.renderArea.extent = swapChainExtent;
            /**
            ���п��Լ�¼ָ�ָ���ĺ����ĺ�����������һ�� vkCmd ǰ׺�� ������Щ�����ķ���ֵ���� void��Ҳ����˵��ָ���¼������ȫ����ǰ�� ���ý����κδ�����
            ���ຯ���ĵ�һ�����������ڼ�¼ָ���ָ�����󡣵ڶ������� ��ʹ�õ���Ⱦ���̵���Ϣ�����һ������������ָ����Ⱦ��������ṩ�� ��ָ��ı�ǣ�������������������ֵ֮һ��
            VK_SUBPASS_CONTENTS_INLINE�� ����Ҫִ�е�ָ�����Ҫָ����У�û�и���ָ�����Ҫִ��
            VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS�� �����Ը���ָ����ָ����Ҫִ�С�
              */
            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            renderPassInfo.clearValueCount = 1;     //ָ����Ǻ�ʹ�õ����ֵ
            renderPassInfo.pClearValues = &clearColor;
            //[13]���һ��������how the drawing commands within the render pass will be provided
            //[13]VK_SUBPASS_CONTENTS_INLINE render pass commands ��Ƕ�� primary command buffer, ���Ҳ���ִ�� secondary command buffers
            //[13]VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERSrender pass commands ���� secondary command buffers ִ��
            //��ʼһ����Ⱦ����
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            //������vkCmd��ͷ�ĵ��ö���¼�ƣ�Record�����������vkQueue��ͷ�Ķ����ύ��Submit�����
            //[13]Basic drawing commands
            //[13]�ڶ�������ָ�����߶�����ͼ�ι��߻��Ǽ�����ߡ�
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            //[13]vertexCount ��ʹû�ж��㻺�������Ӽ����Ͻ�����Ȼ��3������Ҫ����
            //[13]instanceCount ����ʵ������Ⱦ�����������ʵ����ʹ��1
            //[13]firstVertex ���㻺������ƫ����������gl_VertexIndex����Сֵ
            //[13]firstInstance ����ʵ����Ⱦ��ƫ����������gl_InstanceIndex����Сֵ

              /**            ���ˣ������Ѿ��ύ����Ҫͼ�ι���ִ�е�ָ��Լ�Ƭ����ɫ��ʹ�õĸ���            */
            /**
              vkCmdDraw������
              1.��¼��Ҫִ�е�ָ���ָ������
              2. vertexCount��
                ������������û��ʹ�ö��㻺�壬����Ȼ��Ҫָ�������������������εĻ��ơ�
              3.instanceCount������ʵ����Ⱦ��Ϊ 1 ʱ��ʾ������ʵ����Ⱦ
              4.firstVertex�����ڶ�����ɫ������ gl_VertexIndex ��ֵ
              5.firstInstance�����ڶ�����ɫ������ gl_InstanceIndex ��ֵ
              */
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);//��ʼ����ָ����������εĻ��Ʋ���

            //[13]The render pass can now be ended
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) { //������¼ָ�ָ���
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    //[13]---------------------------------------------------------------------------

    //[14]---------------------------------------------------------------------------
    void createSemaphores()
    {
        //����ÿһ֡��Ҫ���ź�������
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        //[14]
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        //[15]
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);


        //[14] Vulkan API ����չ flags �� pNext ������
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        //[14]
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        /**
        Ĭ������£�դ�� (fence) �����ڴ�������δ�����źŵ�״̬�� �����ζ���������û���� vkWaitForFences ��������
        ֮ǰ����դ�� (fence) �źţ�vkWaitForFences �������ý���һֱ���ڵȴ�״̬�� �������ó�ʼ״̬Ϊ�ѷ����ź�
          */
        //[14]if we had rendered an initial frame that finished
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        //[14]
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {//�����ź�����VkFence ����
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                //[14]
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphores for a frame!");
            }
        }

    }


    //[15]-----------------------�첽ִ�е�--�ӽ�������ȡͼ��-------------------------------------------------
    //[15]acquire an image from the swap chain

    /// <summary>
    /// * 1.�ӽ�������ȡһ��ͼ��
    /// *2.��֡���帽��ִ��ָ����е���Ⱦָ��
    ///    * 3.������Ⱦ���ͼ�񵽽��������г��ֲ���
        //������Щ����ÿһ������ͨ��һ�������������õ�, ��ÿ��������ʵ��ִ��ȴ���첽���еġ��������û��ڲ���ʵ�ʽ���ǰ���أ����Ҳ�����ʵ
        //��ִ��˳��Ҳ�ǲ�ȷ���ġ���������Ҫ������ִ���ܰ���һ����˳�����Ծ���Ҫ����ͬ��������
        //����������ͬ���������¼��ķ�ʽ��դ��(fence) ���ź���(semaphore)�����Ƕ��������ͬ��������
        //դ��(fence) ���ź���(semaphore) �Ĳ�֮ͬ���ǣ����ǿ���ͨ������ vkWaitForFences ������ѯդ��(fence) ��״̬�������ܲ�ѯ�ź���
        //(semaphore) ��״̬��
        // ͨ��������ʹ��դ��(fence) ����Ӧ�ó��������Ⱦ��������ͬ����
        // ʹ���ź���(semaphore) ����һ��ָ������ڵĲ���������ָͬ����еĲ�������ͬ����
        //���������Ҫͨ��ָ������еĻ��Ʋ����ͳ��ֲ�������Ȼ��ʹ���ź���(semaphore) ���Ӻ��ʡ�
    /// </summary>
    void drawFrame() {
        //[15]wait for the frame to be finished
        //[15]vkWaitForFences��������һ��fences���飬���ȴ������κ�һ��������fences�ڷ���֮ǰ�����źš�
        //[15]���ﴫ�ݵ�VK_TRUE��ʾҪ�ȴ����е�fences�����Ƕ��ڵ���fences��˵������Ȼ�޹ؽ�Ҫ��
        //[15]����vkAcquireNextImageKHRһ�����������Ҳ��Ҫ��ʱ�����ź�����ͬ��������Ҫͨ��vkresetfines���ý�fence����Ϊunsignaled״̬��
        //vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        //or    

        uint32_t imageIndex;
        //[15]����������ָ���ɻ��ͼ��ĳ�ʱʱ�䣨������Ϊ��λ����
        //[15]����������������ָ����ͬ��������Щ����������ʹ����ͼ��ʱ�����źš� ����ָ��semaphore��fence��ָ��
        //[15]�����������swap chain��ͼ���������
        //vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        //or
        //[15]
        //vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        //or
        //[16]
        /**
          vkAcquireNextImageKHR������
          1.ʹ�õ��߼��豸����
          2.����Ҫ��ȡͼ��Ľ�������
          3.ͼ���ȡ�ĳ�ʱʱ�䣬���ǿ���ͨ��ʹ���޷��� 64 λ�������ܱ�ʾ��
            �������������ͼ���ȡ��ʱ
          4,5.ָ��ͼ����ú�֪ͨ��ͬ������.����ָ��һ���ź��������դ������
            ����ͬʱָ���ź�����դ���������ͬ��������
            ���������ָ����һ������ imageAvailableSemaphore ���ź�������
          6.����������õĽ�����ͼ�������������ʹ������������������ǵ�
            swapChainImages�����е�VkImage���󣬲�ʹ����һ�������ύ��Ӧ��ָ���
          */
          //�ӽ�������ȡһ��ͼ�񣬽�������һ����չ���ԣ�����������صĲ��������� KHR ��һ��չ��׺
        
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);     //�ӽ�������ȡͼƬ
        //[16]VK_ERROR_OUT_OF_DATE_KHR: Swap chain �� surface �����ݣ��޷���������Ⱦ�� ͨ�������ڴ��ڵ�����С֮��
        //[16]VK_SUBOPTIMAL_KHR Swap:  chain�Կ����ڳɹ����ֵ�surface����surface���Բ�����ȫƥ��
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //[18]���SwapChain�ڳ��Ի�ȡͼ��ʱ�ѹ�ʱ�����޷�����ʾ��������ˣ�����Ӧ���������´���SwapChain��������һ��drawFrame���������ԡ�
            recreateSwapChain();
            return;
        }
        //[16]���SwapChain�Ǵ��ŵģ����Ե���recreateSwapChain��������ѡ���������Ϊ�����Ѿ������һ��ͼ��VK_SUCCESS��VK_SUBOPTIMAL_KHR������Ϊ�ǡ��ɹ���    ��
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        /**
        vkWaitForFences �������������ȴ�һ��դ�� (fence) �е�һ����ȫ��դ�� (fence) �����źš�������������Ƕ���ʹ�õ� VK_TRUE
        ��������ָ�����ȴ�������������ָ����դ�� (fence)����������ֻ��һ��դ�� (fence) ��Ҫ�ȴ������Բ�ʹ�� VK_TRUE Ҳ�ǿ��Եġ���
        vkAcquireNextImageKHR ����һ����vkWaitForFences ����Ҳ��һ����ʱ���������ź�����ͬ���ȴ�դ�������źź�������Ҫ���� vkResetFences
        �����ֶ���դ�� (fence) ����Ϊδ�����źŵ�״̬��
          */

        //[15]Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);    //�ȴ����ǵ�ǰ֡��ʹ�õ�ָ������ִ��
        }
        //[15] Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        //�ύ��Ϣ��ָ�����
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
        //or
        //[15]
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        //������Ҫд����ɫ���ݵ�ͼ��,��������ָ���ȴ�ͼ����ߵ������д����ɫ���ŵĹ��߽׶�
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        //[15]ǰ��������ָ����ִ�п�ʼ֮ǰ�ȴ���ЩSemaphore���Լ��ڹ��ߵ��ĸ��׶εȴ���
        /**
        waitSemaphoreCount��pWaitSemaphores ��pWaitDstStageMask ��Ա��������ָ�����п�ʼִ��ǰ��Ҫ�ȴ����ź������Լ���Ҫ�ȴ��Ĺ��߽׶�
          */
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;    //waitStages �����е���Ŀ�� pWaitSemaphores ����ͬ�������ź������Ӧ��

        //[15]ָ��ʵ���ύִ�е�commandBuffer��
        //ָ��ʵ�ʱ��ύִ�е�ָ������
        //����Ӧ���ύ�����Ǹոջ�ȡ�Ľ�����ͼ�����Ӧ��ָ������
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        //[15]ָ����commandBuffer���ִ�к����źŵ�Semaphore��
        //VkSemaphore signalSemaphores[] = { renderFinishedSemaphore};
        //or
        //[15]
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        //ָ��ʵ�ʱ��ύִ�е�ָ������
        //����Ӧ���ύ�����Ǹոջ�ȡ�Ľ�����ͼ�����Ӧ��ָ������
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        //[15] 
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        //[15]���һ�����������˿�ѡ�� fenc, �� command buffer ִ�����ʱ�����������źš�
        //[15]����ʹ���ź�������ͬ�����������Ǵ���VK_NULL_HANDLE��
        //if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to submit draw command buffer!");
        //}
        //or
        //������vkCmd��ͷ�ĵ��ö���¼�ƣ�Record�����������vkQueue��ͷ�Ķ����ύ��Submit�����
         /**
        vkQueueSubmit ����ʹ��vkQueueSubmit�ṹ��������Ϊ����,����ͬʱ�������ύ��.��
        vkQueueSubmit ���������һ��������һ����ѡ��դ������
        ��������ͬ���ύ��ָ���ִ�н�����Ҫ���еĲ�����
        ���������ʹ���ź�������ͬ����û��ʹ��������������ΪVK_NULL_HANDLE
          */

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
        {//ѡ���ʵ���Command buffers�ύ��Queue���ύָ����ͼ��ָ�����
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        
        //����Ⱦ��ͼ�񷵻ظ����������г��ֲ���
        VkPresentInfoKHR presentInfo = {};    //���ó�����Ϣ
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        //[15]�ڳ��֣�Present��֮ǰ�ȴ���Щ�ź���
        presentInfo.waitSemaphoreCount = 1;        //ָ����ʼ���ֲ�����Ҫ�ȴ����ź���
        presentInfo.pWaitSemaphores = signalSemaphores;
        
        //ָ�������ڳ���ͼ��Ľ��������Լ���Ҫ���ֵ�ͼ���ڽ������е�����
        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        //[15]Ϊÿ��swapChainָ�����ֵ�ͼ���ͼ������
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        //[15]check for every individual swap chain if presentation was successful. ��array of VkResult��
         /**
        ���ǿ���ͨ�� pResults ��Ա������ȡÿ���������ĳ��ֲ����Ƿ�ɹ�����Ϣ���������������ֻʹ����һ��������������ֱ��ʹ�ó��ֺ���
        �ķ���ֵ���жϳ��ֲ����Ƿ�ɹ�
          */
        presentInfo.pResults = nullptr; // Optional

        //[15]submits the request to present an image to the swap chain.
        //vkQueuePresentKHR(presentQueue, &presentInfo);
        //or
        //[16]vkQueuePresentKHR �������ؾ�����ͬvkAcquireNextImageKHR����ֵ������ͬ��ֵ��
        //[16]����������£����SwapChain�������ŵģ�����Ҳ�����´���������Ϊ������Ҫ��õĽ����
        result = vkQueuePresentKHR(presentQueue, &presentInfo);         //���󽻻�������ͼ����ֲ���

        //��� framebufferResized ȷ��semaphores����һ��״̬�����������Զ������ȷ�ȴ������źŵ�semaphores�� 
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
        /**
        �������У�������г��򣬹۲�Ӧ�ó�����ڴ�ʹ������� ���Է������ǵ�Ӧ�ó�����ڴ�ʹ����һֱ���������ӡ�����������
        �ǵ� drawFrame �����Ժܿ���ٶ��ύָ���ȴû������һ��ָ���ύʱ�����һ���ύ��ָ���Ƿ��Ѿ�ִ�н�����Ҳ����˵ CPU �ύ
        ָ���� GPU ��ָ��Ĵ����ٶȣ���� GPU ��Ҫ�����ָ������ѻ�������������������£�����ʵ���϶Զ��֡ͬʱʹ������ͬ��
        imageAvailableSemaphore �� renderFinishedSemaphore �ź�����
        ��򵥵Ľ��������һ����ķ�����ʹ�� vkQueueWaitIdle �������ȴ���һ���ύ��ָ�����ִ�У����ύ��һ֡��ָ�
        �����������Ƕ� GPU ������Դ�Ĵ���˷ѡ�ͼ�ι��߿��ܴ󲿷�ʱ�䶼���ڿ���״̬.
          */


        //[15]        //����currentFrame
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        //[15]
        vkQueueWaitIdle(presentQueue);         //�ȴ�һ���ض�ָ����н���ִ��

    }


    //[16]recreate ֮ǰ�� cleanup
    void cleanupSwapChain() {
        for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
        {//����֡����
            vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
        }
        //[16]�������´���Command�أ����൱�˷ѵġ��෴��ʹ��vkfreecandbuffers������������CommandBuffer���Ϳ����������еĳ��������µ�CommandBuffer��
        //��������ָ������
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());        //�ͷ���Դ
        vkDestroyPipeline(device, graphicsPipeline, nullptr);        //���ٹ��߶���
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);    //���ٹ��߲��ֶ���
        vkDestroyRenderPass(device, renderPass, nullptr);            //������Ⱦ���̶���
        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {//����ͼ����ͼ
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);        //���ٽ������������߼��豸�����ǰ����
    }

    //[16] recreate SwapChain�� pipeline must rebuilt
    void recreateSwapChain() {
        //[16]������С��
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {//����Ӧ�ó����ڴ�����С����ֹͣ��Ⱦ��ֱ���������¿ɼ�ʱ�ؽ�������
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        //[16]�ȴ���Դ���ʹ�á�
        vkDeviceWaitIdle(device); //�ȴ��豸���ڿ���״̬�������ڶ����ʹ�ù����н�������ؽ�
        cleanupSwapChain();        //������������
        createSwapChain();        //���´���������
        createImageViews();        //ͼ����ͼ��ֱ�������ڽ�����ͼ��ģ�����Ҳ��Ҫ���ؽ�
        createRenderPass();        //��Ⱦ���������ڽ�����ͼ��ĸ�ʽ,���ڴ�С�ı䲻������ʹ�õĽ�����ͼ���ʽ�ı�
        //�ӿںͲü������ڹ��ߴ���ʱ��ָ�������ڴ�С�ı䣬��Щ����Ҳ��Ҫ�޸�
        //���ǿ���ͨ��ʹ�ö�̬״̬�������ӿںͲü������������ؽ�����
        createGraphicsPipeline();
        //֡�����ָ���ֱ�������ڽ�����ͼ��
        createFramebuffers();
        createCommandBuffers();
    }

    //[16]  Ϊ��̬�������ܽ��������ص�����
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        //[16-2]��ǰ����ָ�븳ֵ
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

//�������е����룬��ѯglfw���Ƿ�����صļ������»��ɿ�����������Ӧ����Ӧ����
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}