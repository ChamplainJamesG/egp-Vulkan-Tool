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

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

const std::vector<const char*> VALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG


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
	void runRenderer(); // The main loop - draw basically.
	void cleanRenderer(); // Cleanup everything on destroy.

	bool checkValidationLayerSupport(); // check for validation layers.
	std::vector<const char*> getRequiredExtensions(); // get the extensions from GLFW

	// member vars
	GLFWwindow* mWindow; // The window that we see.

	VkInstance mVkInstance; // Instance that allows us to interface w/ vulkan.
	VkDebugUtilsMessengerEXT mDebugMessenger; // allows for debug callback with validation layers. 

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
