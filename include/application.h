#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <set>
#include <array>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <algorithm>
#include <stdexcept>

#include <cstring>
#include <cstdlib>

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

const uint32_t WIDTH = 600;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

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

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription get_binding_description()
    {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        return attribute_descriptions;
    }
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0};

class Application
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

    VkSwapchainKHR swap_chain;
    std::vector<VkImage> swap_chain_images;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    std::vector<VkImageView> swap_chain_image_views;
    std::vector<VkFramebuffer> swap_chain_framebuffers;

    VkRenderPass render_pass;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;

    VkCommandPool command_pool;

    /*Driver developers recommend that you also store multiple buffers, like the vertex and index buffer, 
    into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. 
    The advantage is that your data is more cache friendly in that case, because it's closer together. 
    It is even possible to reuse the same chunk of memory for multiple resources 
    if they are not used during the same render operations, provided that their data is refreshed, of course. 
    This is known as aliasing and some Vulkan functions have explicit flags to specify that you want to do this.
    https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer*/

    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    std::vector<VkBuffer> uniform_buffers;
    std::vector<VkDeviceMemory> uniform_buffers_memory;

    VkDescriptorPool descriptor_pool;
    std::vector<VkDescriptorSet> descriptor_sets;

    std::vector<VkCommandBuffer> command_buffers;

    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> render_finished_semaphores;
    std::vector<VkFence> in_flight_fences;
    std::vector<VkFence> images_in_flight;
    size_t current_frame = 0;

    bool framebuffer_resized = false;

    void init_window();
    void init_vulkan();
    void main_loop();
    void cleanup();

    void clean_swap_chain();
    void recreate_swap_chain();

    void create_instance();
    void create_surface();

    void pick_physical_device();
    bool is_device_suitable(VkPhysicalDevice device);

    void create_logical_device();
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

    void create_swap_chain();
    void create_image_views();
    void create_render_pass();
    void create_descriptor_layout();
    void create_graphics_pipeline();
    void create_framebuffers();
    void create_command_pool();

    void create_vertex_buffer();
    void create_index_buffer();
    void create_uniform_buffers();

    void create_descriptor_pool();
    void create_descriptor_sets();

    void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                       VkBuffer &buffer, VkDeviceMemory &buffer_memory);
    void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
    uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

    void create_command_buffers();

    void update_uniform_buffer(uint32_t current_image);
    void draw_frame();
    void create_sync_objects();

    VkShaderModule create_shader_module(const std::vector<char> &code);
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);
    void setup_debug_messenger();
    bool check_validation_layer_support();
    bool check_device_extension_support(VkPhysicalDevice device);

    std::vector<const char *> get_required_extensions();

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
        app->framebuffer_resized = true;
    }

    static std::vector<char> read_file(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("failed to open file!");

        size_t file_size = (size_t)file.tellg();
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);
        file.close();

        return buffer;
    }

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