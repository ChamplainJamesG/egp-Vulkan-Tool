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
#include <algorithm>
#include <cstdlib>
#include <fstream>
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

const std::vector<const char*> VALIDATION_LAYERS = 
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DEVICE_EXTENSIONS =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

	// used to check if we can present to the surface, and can just do graphics stuff.
	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

// store data for swap chain properties & stuff
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities; // basic surface capabilities (min/max number of images in swapchain, min/max width / height.
	std::vector<VkSurfaceFormatKHR> formats; // things like pixel format, color space.
	std::vector<VkPresentModeKHR> presentModes; // available presentation modes.
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
	bool checkDeviceExtensionSupport(VkPhysicalDevice device); // do an additional check for device extensions (i.e, can it present)
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device); // We need to submit different queues to command buffers.
	void createLogicalDevice();
	void createSurface();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	// helper functions to set up swap chain 
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();
	// end ^^^
	void createRenderPass(); // create a render pass for graphics pipeline to use.
	void createGraphicsPipeline(); // we have to make our own graphics pipeline. 
	VkShaderModule createShaderModule(const std::vector<char>& code); // helper to create shader modules for vulkan from the shader code.
	void createFrameBuffers(); // Create framebuffers
	void createCommandPool(); // Create pool for command buffers
	void createCommandBuffers();
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
	VkQueue mPresentQueue; // queue for commands for presenting to the surface.
	VkSwapchainKHR mSwapChain; // the swap chain - list of images that are ready to be rendered.
	std::vector<VkImage> mSwapChainImages; // list of pointers / handles to get images back from the swap chain.
	VkFormat mSwapChainImageFormat; // used to store the swap chain image format for later (i.e recreation of swapchain)
	VkExtent2D mSwapChainExtent; // same as above.
	std::vector<VkImageView> mSwapChainImageViews; // how we can access an image.
	VkPipelineLayout mPipelineLayout; // the vulkan graphics pipeline.
	VkRenderPass mRenderPass; // render pass storage
	VkPipeline mGraphicsPipeline; // Stores the graphics pipeline & all stages.
	std::vector<VkFramebuffer> mSwapChainFrameBuffers; // storage of frame buffers
	VkCommandPool mCommandPool; // pool for command buffers
	std::vector<VkCommandBuffer> mCommandBuffers; // list of command buffers

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

	// Helper function to read in files (mostly shaders though). 
	static std::vector<char> readFile(const std::string& fileName)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open a file.");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
};


#endif // !VK_RENDERER_H
