#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

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

        void initWindow() {
            /**
             * Initializes the GLFW library with an
             * OpenGL context.
            */
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }
        
        void initVulkan() {

        }

        void mainLoop() {
            /**
             * Run the application.
            */
           while ((!glfwWindowShouldClose(window))) {
                glfwPollEvents();
           }
        }

        void cleanup() {
            /**
             * Clean up resources by destroying the GLFW application.
            */
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