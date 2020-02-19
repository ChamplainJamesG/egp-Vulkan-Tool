#ifndef VK_RENDERER_H
#define VK_RENDERER_H

// forces GLFW to include vulkan with its header.
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include <iostream>

#include <vector>
#include <optional>

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

const std::vector<const char*> VALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG

// store all queue families for commands for the buffer.
// because we have to store ints, we use optional to check whether it's a valid index
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily; // queue family for presentation, letting us present stuff on the surface.

	bool isSomething()
	{
		return graphicsFamily.has_value();
	}
};


class VulkanRenderer
{
public:
	void run();

private:
	// functions
	void initGLFWWindow(); // init the glfw window
	void initVulkan(); // Init Vulkan
	void createVkInstance(); // Create a vulkan instance
	void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT &createInfo); // we can populate the messenger, and then that lets us do calls for instance creation and destruction.
	void createDebugMessenger();
	void findPhysicalDevice(); // find a suitable graphics card to run on
	bool isDeviceSuitable(VkPhysicalDevice dev); // check whether a GPU is suitable or not for Vk.
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device); // We need to submit different queues to command buffers.
	void createLogicalDevice();
	void createSurface();
	void runRenderer(); // The main loop - draw basically.
	void cleanRenderer(); // Cleanup everything on destroy.

	bool checkValidationLayerSupport(); // check for validation layers.
	std::vector<const char*> getRequiredExtensions(); // get the extensions from GLFW

	// member vars
	GLFWwindow* mWindow; // The window that we see.

	VkInstance mVkInstance; // Instance that allows us to interface w/ vulkan.
	VkDebugUtilsMessengerEXT mDebugMessenger; // allows for debug callback with validation layers. 
	VkPhysicalDevice mPhysicalDevice; // the graphcis card to interface with.
	VkDevice mLogicalDevice; // the logical device that lets us interface with the physical device.
	VkQueue mGraphicsQueue; // the graphics queue for graphics things to submit to the command buffer.
	VkSurfaceKHR mSurface; // Windows surface to draw to. Linux needs another one. Mac probably needs moltenVk.

	// static and other members down here.
	// we add macros to make sure vulkan can call this and we have to like "register" it.
	// validation layers.
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};


#endif // !VK_RENDERER_H
