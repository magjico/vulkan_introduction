#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const char **glfwExtensions;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifndef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication
{
    public:
    
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:

        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        /**
         * Debug callback function
         * 
         * @param[out] messageSeverity specifies the severity of the message. 
         * @param[out] messageType this parameter define how events happened.
         * @param[in] pCallbackData refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of the message itself.
         * @param[out] pUserData contains a pointer that was specified during the setup of the callback and allows you to pass your own data to it.
         * 
         * @return A boolean that indicates if the Vulkan call that triggered the validation layer message should be aborted. 
        */
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
        )
        {
            if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                // Message is important enough to show
                std::cout << pCallbackData->pMessage << std::endl;
            }
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        /**
         * Get all required glfw extensions.
         * 
         * @return a vector that contain the names of all required glfw extensions.
        */
        std::vector<const char*> getRequiredExtensions()
        {
            
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enableValidationLayers)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            
            return extensions;
        }

        /**
         * Checking if the validation layers are supported:
         * https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
         * 
         * @return true if the validation layers are supported else false.
        */
        bool checkValidationLayerSupport()
        {
            std::cout << "DEBUG MOD: ON" << std::endl;

            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    std::cout << "layer " << layerName << " was not found" << std::endl;
                    return false;
                }

            }

            return true;
        }

        /**
         * Create a vulkan instance to optimize our specific application.
        */
        void createInstance()
        {
            if (enableValidationLayers && !checkValidationLayerSupport())
                throw std::runtime_error("validation layers requested, but not available!");
            
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "Hello Triangle";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "No Engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
                throw std::runtime_error("failed to create instance!"); 
            }
        }

        /**
         * Initializes the GLFW library with an OpenGL context.
        */
        void initWindow()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }
        
        /**
         * Initialize a Vulkan instance.
        */
        void initVulkan()
        {
            createInstance();
            setupDebugMessenger();
        }


        /**
         * Create a separate debug utils messenger specifically for 
         * vkCreateInstance and vkDestroyInstance
        */
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
        {
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;
        }

        /**
         * Create a debug callback
         * 
         * @throw runtime_error if we failed to setup the debug messenger
        */
        void setupDebugMessenger()
        {
            if (!enableValidationLayers) return;

            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            populateDebugMessengerCreateInfo(createInfo);

            if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
                throw std::runtime_error("failed to setup the debug messenger!"); 
        }

        /**
         * Configure validation layer messages and debug callbacks.
         * 
         * @param[out] instance the vulkan instance.
         * @param[in] pCreateInfo a pointer to a struct specifying parameters of a newly created debug messenger.
         * @param[in] pAllocator a pointer to a struct that containing callback function pointers for memory allocation.
         * @param[out] pDebugMessenger a pointer to the vulkan debug messenger.
         * 
         * @return a configure validation layer.
        */
        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugUtilsMessengerEXT *pDebugMessenger
        )
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                instance, "vkCreateDebugUtilsMessengerEXT"
            );
            if (func != nullptr)
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        /**
         * clean up the VkDebugUtilsMessengerEXT object.
         * 
         * @param[out] instance the vulkan instance.
         * @param[out] debugMessenger the vulkan debug messenger.
         * @param[in] pAllocator a pointer to a struct that containing callback function pointers for memory allocation.
        */
        void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks *pAllocator
        )
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(instance, debugMessenger, pAllocator);
        }

        /**
         * Run the application.
        */
        void mainLoop()
        {
            while ((!glfwWindowShouldClose(window)))
                glfwPollEvents();
        }

        /**
         * Clean up resources by destroying the GLFW & Vulkan applications.
        */
        void cleanup()
        {
            if (enableValidationLayers)
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            
            vkDestroyInstance(instance, nullptr);

            glfwDestroyWindow(window);

            glfwTerminate();

            std::cout << "Application Terminated" << std::endl;
        }
};

int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}