#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <set>
#include <array>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
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

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/sponza.obj";
const std::string TEXTURE_PATH = "textures/null.png";

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validation_layers = {
    "VK_LAYER_KHRONOS_validaton"};

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
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 txr_coord;

    static VkVertexInputBindingDescription get_binding_description()
    {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[2].offset = offsetof(Vertex, txr_coord);

        return attribute_descriptions;
    }
    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && txr_coord == other.txr_coord;
    }
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >>
                    1) ^
                   (hash<glm::vec2>()(vertex.txr_coord) << 1);
        }
    };
}

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

    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;

    /*Driver developers recommend that you also store multiple buffers, like the vertex and index buffer, 
    into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. 
    The advantage is that your data is more cache friendly in that case, because it's closer together. 
    It is even possible to reuse the same chunk of memory for multiple resources 
    if they are not used during the same render operations, provided that their data is refreshed, of course. 
    This is known as aliasing and some Vulkan functions have explicit flags to specify that you want to do this.
    https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer*/

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
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

    //frame timing (s)
    float delta = 0.0f;
    float t_last_frame = 0.0f;
    float t_last_monitor = 0.0f;

    //camera values
    glm::vec3 camera_pos = {0.0f, 4.0f, 0.0f};
    glm::vec3 camera_forward = {0.0f, -1.0f, 0.0};
    glm::vec3 camera_up = {0.0f, 0.0f, 1.0f};
    float camera_pitch = 0.0f;
    float camera_yaw = -90.0f;
    float camera_speed = 0.0f;

    bool m_captured = true;
    bool m_init = true;
    float m_lastx = (float)WIDTH / 2.0f;
    float m_lasty = (float)HEIGHT / 2.0f;

    void load_model();
    void process_input();
    void process_timing(bool show_fps);

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

    void create_depth_resources();
    VkFormat find_depth_format();
    VkFormat find_supported_format(const std::vector<VkFormat> &candidates,
                                   VkImageTiling tiling, VkFormatFeatureFlags features);
    bool has_stencil_component(VkFormat format);

    void create_texture_image();
    void create_texture_image_view();
    void create_texture_sampler();

    VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
    void create_image(uint32_t width, uint32_t height, VkFormat format,
                      VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkImage &image, VkDeviceMemory &imageMemory);
    void transition_image_layout(VkImage image, VkFormat format,
                                 VkImageLayout old_layout, VkImageLayout new_layout);
    void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void create_vertex_buffer();
    void create_index_buffer();
    void create_uniform_buffers();

    void create_descriptor_pool();
    void create_descriptor_sets();

    void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                       VkBuffer &buffer, VkDeviceMemory &buffer_memory);
    VkCommandBuffer begin_single_time_commands();
    void end_single_time_commands(VkCommandBuffer command_buffer);
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

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
        void *p_user_data)
    {
        std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;

        return VK_FALSE;
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

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
        app->framebuffer_resized = true;
    }

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            if (app->m_captured)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            app->m_captured = !app->m_captured;
        }
    }

    static void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    {
        auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

        if (!app->m_captured)
            return;

        if (app->m_init)
        {
            app->m_lastx = xpos;
            app->m_lasty = ypos;
            app->m_init = false;
        }

        float x_offset = xpos - app->m_lastx;
        float y_offset = ypos - app->m_lasty;
        app->m_lastx = xpos;
        app->m_lasty = ypos;

        float sensitivity = 0.1f;
        x_offset *= sensitivity;
        y_offset *= sensitivity;

        app->camera_yaw -= x_offset;
        app->camera_pitch -= y_offset;

        app->camera_pitch = std::clamp(app->camera_pitch, -89.0f, 89.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(app->camera_yaw)) * cos(glm::radians(app->camera_pitch));
        direction.y = sin(glm::radians(app->camera_yaw)) * cos(glm::radians(app->camera_pitch));
        direction.z = sin(glm::radians(app->camera_pitch));
        app->camera_forward = glm::normalize(direction);
    }
};

#endif /*APPLICATION_H*/