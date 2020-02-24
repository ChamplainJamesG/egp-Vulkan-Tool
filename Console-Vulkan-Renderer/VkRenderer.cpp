#include "VkRenderer.h"
// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
// Need to make our own creation function, because validation layers are not automatically loaded 
VkResult createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

// also need to make our own destruction function.
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

void VulkanRenderer::run()
{
	initGLFWWindow();
	initVulkan();
	
	runRenderer();
	cleanRenderer();
}

void VulkanRenderer::initGLFWWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sick Vulkan Window", nullptr, nullptr);

}

void VulkanRenderer::initVulkan()
{
	createVkInstance();
	createDebugMessenger();
	createSurface();
	findPhysicalDevice();
	createLogicalDevice();
}

// instance
// create a single VkInstance
void VulkanRenderer::createVkInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
		throw std::runtime_error("Validation layers are enabled, but none found or enabled.");

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Very cool vulkan application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Very cool engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(0, &glfwExtensionsCount, 0);

	std::vector<VkExtensionProperties> extensions(glfwExtensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionsCount, extensions.data());

	std::cout << "Available extensions for Vk:" << std::endl;

	for (const auto& ext : extensions)
		std::cout << ext.extensionName << std::endl;

	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);


	auto func_exts = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(func_exts.size());
	createInfo.ppEnabledExtensionNames = func_exts.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

		populateDebugMessenger(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &mVkInstance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance. Very stinky.");
}

void VulkanRenderer::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback; // pointer to callback function
	createInfo.pUserData = nullptr;
}

// validation layers
// create debug messenger that can display errors for validation layers.
void VulkanRenderer::createDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessenger(createInfo);

	if (createDebugUtilsMessengerExt(mVkInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
		throw std::runtime_error("Failed to create the debug messenger.");
}

void VulkanRenderer::findPhysicalDevice()
{
	uint32_t deviceCt = 0;
	vkEnumeratePhysicalDevices(mVkInstance, &deviceCt, nullptr);

	if (deviceCt == 0)
		throw std::runtime_error("No GPU on the device with Vulkan support. Panic!");

	// fill the vector with data now based on the devices we found that were suitable.
	std::vector<VkPhysicalDevice> devices(deviceCt);
	vkEnumeratePhysicalDevices(mVkInstance, &deviceCt, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			mPhysicalDevice = device;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a GPU. Panic!");
}

// later on down the line, I could create something that finds a better GPU based on "score"
bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device)
{
	//VkPhysicalDeviceProperties deviceProperties;
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

	QueueFamilyIndices ind = findQueueFamilies(device);
	return ind.isSomething();
}

QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCt;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCt, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCt);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCt, queueFamilies.data());

	// now iterate through Vk's queue family properties structure we just made to find things we need.
	// specifically, looking for queue families that support the graphics bit flag, but could be anything.

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		if (indices.isSomething())
			break;

		++i;
	}

	return indices;
}

void VulkanRenderer::createLogicalDevice()
{
	// need to make queue info for the logical device, since it also has a lot of queues.
	QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	// can have 0 - 1 priority values of how queues should be submitted to the command buffer.
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// Get the features from the physical device to use from the logical device
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device. That's rough buddy.");

	// get the queue now that everyone is set up.
	vkGetDeviceQueue(mLogicalDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
}

void VulkanRenderer::createSurface()
{
	// we can let GLFW handle all the hard stuff for creating a surface, which is nice.
	if (glfwCreateWindowSurface(mVkInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS)
		throw std::runtime_error("Couldn't create the window surface. Everything is broken.");
}

void VulkanRenderer::runRenderer()
{
	while (!glfwWindowShouldClose(mWindow))
		glfwPollEvents();
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> possibleLayers (layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, possibleLayers.data());

	std::cout << "All possible validation layers: " << std::endl;
	for (const auto& layer : possibleLayers)
		std::cout << layer.layerName << " : " << layer.description << std::endl;

	for (const char* layerName : VALIDATION_LAYERS)
	{
		bool layerFound = false;

		for(const auto& layerProperties : possibleLayers)
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}

		if (!layerFound)
			return false;
	}

	return true;
}

std::vector<const char*> VulkanRenderer::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void VulkanRenderer::cleanRenderer()
{
	vkDestroyDevice(mLogicalDevice, nullptr);
	if (enableValidationLayers)
		destroyDebugUtilsMessengerEXT(mVkInstance, mDebugMessenger, nullptr);
	vkDestroySurfaceKHR(mVkInstance, mSurface, nullptr);
	vkDestroyInstance(mVkInstance, nullptr);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}