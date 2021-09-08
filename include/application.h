#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <vector>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char *> validation_layers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkResult create_debug_utils_messengerEXT( //proxy creation func
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger);

void destroy_debug_utils_messengerEXT( //proxy destroy func
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator);

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete()
    {
        return graphics_family.has_value() &&
               present_family.has_value();
    }
};

class HelloTriangleApplication
{
public:
    void run();

private:
    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphics_queue;
    VkQueue present_queue;

    void init_window();
    void init_vulkan();
    void main_loop();
    void cleanup();

    void create_instance();
    void create_surface();

    void pick_physical_device();
    bool is_device_suitable(VkPhysicalDevice device);
    void create_logical_device();
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);
    void setup_debug_messenger();
    bool check_validation_layer_support();
    bool check_device_extension_support(VkPhysicalDevice device);
    std::vector<const char *> get_required_extensions();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
        void *p_user_data)
    {
        std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;

        return VK_FALSE;
    }
};

#endif /*APPLICATION_H*/