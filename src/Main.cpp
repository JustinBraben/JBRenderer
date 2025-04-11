/*
 * Copyright (c) 2023 TU Wien, Institute of Visual Computing & Human-Centered Technology.
 */

// Include our framework and the Vulkan headers:
#include "VulkanLaunchpad.h"
#include <vulkan/vulkan.h>

// Include some local helper functions:
#include "VulkanHelpers.h"
#include "Teapot.h"

// Include functionality from the standard library:
#include <vector>
#include <unordered_map>
#include <limits>

// Include the camera header:
#include "Camera.h"

/* ------------------------------------------------ */
// Some more little helpers directly declared here:
// (Definitions of functions below the main function)
/* ------------------------------------------------ */

/*!
 *	This callback function gets invoked by GLFW whenever a GLFW error occured.
 */
void errorCallbackFromGlfw(int error, const char *description);

/*!
 *	Function that is invoked by GLFW to handle key events like key presses or key releases.
 *	If the ESC key has been pressed, the window will be marked that it should close.
 */
void handleGlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods);

/*!
 *	Function that can be used to query whether or not currently, i.e. NOW, a certain button
 *  is pressed down, or not.
 *  @param	glfw_key_code	One of the GLFW key codes.
 *                          I.e., use one of the defines that start with GLFW_KEY_*
 *  @return True if the given key is currently pressed down, false otherwise (i.e. released).
 */
bool isKeyDown(int glfw_key_code);

/*!
 *	Determine the Vulkan instance extensions that are required by GLFW and Vulkan Launchpad.
 *	Required extensions from both sources are combined into one single vector (i.e., in
 *	contiguous memory, so that they can easily be passed to:
 *  VkInstanceCreateInfo::enabledExtensionCount and to VkInstanceCreateInfo::ppEnabledExtensionNames.
 *	@return     A std::vector of const char* elements, containing all required instance extensions.
 *	@example    std::vector<const char*> extensions = getRequiredInstanceExtensions();
 *	            VkInstanceCreateInfo create_info    = {};
 *	            create_info.enabledExtensionCount   = extensions.size();
 *	            create_info.ppEnabledExtensionNames = extensions.data();
 */
std::vector<const char *> getRequiredInstanceExtensions();

/*!
 *	Based on the given physical device and the surface, select a queue family which supports both,
 *	graphics and presentation to the given surface. Return the INDEX of an appropriate queue family!
 *	@return		The index of a queue family which supports the required features shall be returned.
 */
uint32_t selectQueueFamilyIndex(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

/* ------------------------------------------------ */
// Main
/* ------------------------------------------------ */

int main(int argc, char **argv)
{
	VKL_LOG(":::::: WELCOME TO VULKAN LAUNCHPAD ::::::");

	// Install a callback function, which gets invoked whenever a GLFW error occurred:
	glfwSetErrorCallback(errorCallbackFromGlfw);

	// Initialize GLFW:
	if (!glfwInit())
	{
		VKL_EXIT_WITH_ERROR("Failed to init GLFW");
	}

	/* --------------------------------------------- */
	// Task 1.1: Create a Window with GLFW
	/* --------------------------------------------- */
	constexpr int window_width = 800;
	constexpr int window_height = 800;
	constexpr bool fullscreen = false;
	constexpr char *window_title = "Tutorial Window";

	// Use a monitor if we'd like to open the window in fullscreen mode:
	GLFWmonitor *monitor = nullptr;
	if (fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();
	}

	// Set some window settings before creating the window:
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No need to create a graphics context for Vulkan
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Get a valid window handle and assign to window:
	GLFWwindow *window = glfwCreateWindow(window_width, window_height, window_title, monitor, NULL);

	if (!window)
	{
		VKL_LOG("If your program reaches this point, that means two things:");
		VKL_LOG("1) Project setup was successful. Everything is working fine.");
		VKL_LOG("2) You haven't implemented the first task, which is creating a window with GLFW.");
		VKL_EXIT_WITH_ERROR("No GLFW window created.");
	}
	VKL_LOG("Task 1.1 done.");

	// Set up a key callback via GLFW here to handle keyboard user input:
	glfwSetKeyCallback(window, handleGlfwKeyCallback);

	/* --------------------------------------------- */
	// Task 1.2: Create a Vulkan Instance
	/* --------------------------------------------- */
	VkInstance vk_instance = VK_NULL_HANDLE;

	// Describe some meta data about this application, and define which Vulkan API version is required:
	VkApplicationInfo application_info = {};					 // Zero-initialize every member
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // Set this struct instance's type
	application_info.pEngineName = "Vulkan Launchpad";			 // Set some properties...
	application_info.engineVersion = VK_MAKE_API_VERSION(0, 2023, 1, 0);
	application_info.pApplicationName = "An Introduction to Vulkan";
	application_info.applicationVersion = VK_MAKE_API_VERSION(0, 2023, 1, 1);
	application_info.apiVersion = VK_API_VERSION_1_1; // Your system needs to support this Vulkan API version.

	// We'll require some extensions (e.g., for presenting something on a window surface, and more):
	std::vector<const char *> required_extensions = getRequiredInstanceExtensions();

	// Layers enable additional functionality. We'd like to enable the standard validation layer,
	// so that we get meaningful and descriptive error messages whenever we messed up something:
	if (!hlpIsInstanceLayerSupported("VK_LAYER_KHRONOS_validation"))
	{
		VKL_EXIT_WITH_ERROR("Validation layer \"VK_LAYER_KHRONOS_validation\" is not supported.");
	}
	VKL_LOG("Validation layer \"VK_LAYER_KHRONOS_validation\" is supported.");
	std::vector<const char *> enabled_layers{"VK_LAYER_KHRONOS_validation"};

	// Tie everything from above together in an instance of VkInstanceCreateInfo:
	VkInstanceCreateInfo instance_create_info = {};						 // Zero-initialize every member
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // Set this struct instance's type
	instance_create_info.pApplicationInfo = &application_info;
	// Hook in required_extensions using VkInstanceCreateInfo::enabledExtensionCount and VkInstanceCreateInfo::ppEnabledExtensionNames
	instance_create_info.enabledExtensionCount = required_extensions.size();
	instance_create_info.ppEnabledExtensionNames = required_extensions.data();

	// Hook in enabled_layers using VkInstanceCreateInfo::enabledLayerCount and VkInstanceCreateInfo::ppEnabledLayerNames
	instance_create_info.enabledLayerCount = enabled_layers.size();
	instance_create_info.ppEnabledLayerNames = enabled_layers.data();

	// Use vkCreateInstance to create a vulkan instance handle
	VkResult result = vkCreateInstance(&instance_create_info, nullptr, &vk_instance);
	VKL_CHECK_VULKAN_RESULT(result);

	if (!vk_instance)
	{
		VKL_EXIT_WITH_ERROR("No VkInstance created or handle not assigned.");
	}
	VKL_LOG("Task 1.2 done.");

	/* --------------------------------------------- */
	// Task 1.3: Create a Vulkan Window Surface
	/* --------------------------------------------- */
	VkSurfaceKHR vk_surface = VK_NULL_HANDLE;

	// Use glfwCreateWindowSurface to create a window surface
	result = glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface);
	VKL_CHECK_VULKAN_RESULT(result);

	if (!vk_surface)
	{
		VKL_EXIT_WITH_ERROR("No VkSurfaceKHR created or handle not assigned.");
	}
	VKL_LOG("Task 1.3 done.");

	/* --------------------------------------------- */
	// Task 1.4 Pick a Physical Device
	/* --------------------------------------------- */
	VkPhysicalDevice vk_physical_device = VK_NULL_HANDLE;

	// Use vkEnumeratePhysicalDevices to get all the available physical device handles
	uint32_t num_physical_devices;
	result = vkEnumeratePhysicalDevices(vk_instance, &num_physical_devices, nullptr);
	assert(num_physical_devices > 0);
	std::vector<VkPhysicalDevice> physical_devices(num_physical_devices);
	vkEnumeratePhysicalDevices(vk_instance, &num_physical_devices, physical_devices.data());
	auto index = hlpSelectPhysicalDeviceIndex(physical_devices, vk_surface);
	vk_physical_device = physical_devices[index];
	VKL_CHECK_VULKAN_RESULT(result);

	if (!vk_physical_device)
	{
		VKL_EXIT_WITH_ERROR("No VkPhysicalDevice selected or handle not assigned.");
	}
	VKL_LOG("Task 1.4 done.");

	/* --------------------------------------------- */
	// Task 1.5: Select a Queue Family
	/* --------------------------------------------- */

	// Find a suitable queue family and assign its index
	uint32_t selected_queue_family_index = selectQueueFamilyIndex(vk_physical_device, vk_surface);

	// Sanity check if we have selected a valid queue family index:
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, nullptr);
	if (selected_queue_family_index >= queue_family_count)
	{
		VKL_EXIT_WITH_ERROR("Invalid queue family index selected.");
	}
	VKL_LOG("Task 1.5 done.");

	/* --------------------------------------------- */
	// Task 1.6: Create a Logical Device and Get Queue
	/* --------------------------------------------- */
	VkDevice vk_device = VK_NULL_HANDLE;
	VkQueue vk_queue = VK_NULL_HANDLE;

	constexpr float queue_priority = 1.0f;

	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = selected_queue_family_index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &queue_priority;

	// Create an instance of VkDeviceCreateInfo and use it to create one queue
	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos = &queue_create_info;
	device_create_info.queueCreateInfoCount = 1;

	// Enable the swapchain extension
	const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	device_create_info.enabledExtensionCount = 1;
	device_create_info.ppEnabledExtensionNames = device_extensions;

	// Create the logical device
	result = vkCreateDevice(vk_physical_device, &device_create_info, nullptr, &vk_device);
	VKL_CHECK_VULKAN_RESULT(result);

	if (!vk_device)
	{
		VKL_EXIT_WITH_ERROR("No VkDevice created or handle not assigned.");
	}

	// After device creation, use vkGetDeviceQueue to get the created queue
	vkGetDeviceQueue(vk_device, selected_queue_family_index, 0, &vk_queue);

	if (!vk_queue)
	{
		VKL_EXIT_WITH_ERROR("No VkQueue selected or handle not assigned.");
	}
	VKL_LOG("Task 1.6 done.");

	/* --------------------------------------------- */
	// Task 1.7: Create Swap Chain
	/* --------------------------------------------- */
	VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;

	VkSurfaceCapabilitiesKHR surface_capabilities = hlpGetPhysicalDeviceSurfaceCapabilities(vk_physical_device, vk_surface);

	// Build the swapchain config struct:
	VkSwapchainCreateInfoKHR swapchain_create_info = {};
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = vk_surface;
	swapchain_create_info.minImageCount = surface_capabilities.minImageCount;
	swapchain_create_info.imageArrayLayers = 1u;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.preTransform = surface_capabilities.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// Set queue family index
	swapchain_create_info.queueFamilyIndexCount = 1;
	swapchain_create_info.pQueueFamilyIndices = &selected_queue_family_index;

	// Set image format and color space
	VkSurfaceFormatKHR surface_format = hlpGetSurfaceImageFormat(vk_physical_device, vk_surface);
	swapchain_create_info.imageFormat = surface_format.format;
	swapchain_create_info.imageColorSpace = surface_format.colorSpace;

	// Set image extent
	swapchain_create_info.imageExtent = {window_width, window_height};

	// Set present mode
	swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; // This is guaranteed to be supported

	// Create the swapchain
	result = vkCreateSwapchainKHR(vk_device, &swapchain_create_info, nullptr, &vk_swapchain);
	VKL_CHECK_VULKAN_RESULT(result);

	if (!vk_swapchain)
	{
		VKL_EXIT_WITH_ERROR("No VkSwapchainKHR created or handle not assigned.");
	}

	// Create a vector of VkImages with enough memory for all the swap chain's images:
	std::vector<VkImage> swap_chain_images(surface_capabilities.minImageCount);
	// Use vkGetSwapchainImagesKHR to write VkImage handles into swap_chain_images.data()
	uint32_t image_count = surface_capabilities.minImageCount;
	result = vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, swap_chain_images.data());
	VKL_CHECK_VULKAN_RESULT(result);

	if (swap_chain_images.empty())
	{
		VKL_EXIT_WITH_ERROR("Swap chain images not retrieved.");
	}
	VKL_LOG("Task 1.7 done.");

	/* --------------------------------------------- */
	// Task 1.8: Initialize Vulkan Launchpad
	/* --------------------------------------------- */

	// Gather swapchain config as required by the framework:
	VklSwapchainConfig swapchain_config = {};
	swapchain_config.imageExtent = swapchain_create_info.imageExtent;
	swapchain_config.swapchainHandle = vk_swapchain;
	// Clear any previous data
	swapchain_config.swapchainImages.clear();

	for (VkImage vk_image : swap_chain_images)
	{
		VklSwapchainFramebufferComposition framebufferData;
		// Fill the data for the color attachment
		framebufferData.colorAttachmentImageDetails.imageHandle = vk_image;
		framebufferData.colorAttachmentImageDetails.imageFormat = swapchain_create_info.imageFormat;
		framebufferData.colorAttachmentImageDetails.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Set cornflower blue as clear color (0.39f, 0.58f, 0.93f, 1.0f)
		framebufferData.colorAttachmentImageDetails.clearValue.color.float32[0] = 0.39f;
		framebufferData.colorAttachmentImageDetails.clearValue.color.float32[1] = 0.58f;
		framebufferData.colorAttachmentImageDetails.clearValue.color.float32[2] = 0.93f;
		framebufferData.colorAttachmentImageDetails.clearValue.color.float32[3] = 1.0f;

		// We don't need the depth attachment now
		framebufferData.depthAttachmentImageDetails.imageHandle = VK_NULL_HANDLE;

		// Add it to the vector
		swapchain_config.swapchainImages.push_back(framebufferData);
	}

	// Init the framework:
	if (!vklInitFramework(vk_instance, vk_surface, vk_physical_device, vk_device, vk_queue, swapchain_config))
	{
		VKL_EXIT_WITH_ERROR("Failed to init Vulkan Launchpad");
	}
	VKL_LOG("Task 1.8 done.");

	/* --------------------------------------------- */
	// Create a camera to navigate the scene
	/* --------------------------------------------- */
	VklCameraHandle camera = vklCreateCamera(window);

	// Position the camera a bit away from the origin
	double initial_x, initial_y;
	glfwGetCursorPos(window, &initial_x, &initial_y);
	vklUpdateCamera(camera, initial_x, initial_y, 6.0f, false, false);

	/* --------------------------------------------- */
	// Task 1.9:  Implement the Render Loop
	/* --------------------------------------------- */
	// Create teapot geometry before entering the render loop
	teapotCreateGeometryAndBuffers();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // Handle user input

		// Update camera
		vklUpdateCamera(camera);

		// Wait for the next swap chain image
		vklWaitForNextSwapchainImage();

		// Start recording commands
		vklStartRecordingCommands();

		// Draw the teapot with the camera's view projection matrix
		glm::mat4 view_proj_matrix = vklGetCameraViewProjectionMatrix(camera);
		teapotDraw();

		// End recording commands and submit them
		vklEndRecordingCommands();

		// Present the rendered image to the screen
		vklPresentCurrentSwapchainImage();
	}

	// Wait for all GPU work to finish before cleaning up:
	vkDeviceWaitIdle(vk_device);

	/* --------------------------------------------- */
	// Task 1.10: Cleanup
	/* --------------------------------------------- */
	// Clean up teapot resources
	teapotDestroyBuffers();

	// Destroy the camera
	vklDestroyCamera(camera);

	vklDestroyFramework();

	// Destroy the swapchain
	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);

	// Destroy the device
	vkDestroyDevice(vk_device, nullptr);

	// Destroy the surface
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);

	// Destroy the instance
	vkDestroyInstance(vk_instance, nullptr);

	// Destroy the window
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	return EXIT_SUCCESS;
}

/* ------------------------------------------------ */
// Definitions of little helpers defined above main:
/* ------------------------------------------------ */

void errorCallbackFromGlfw(int error, const char *description)
{
	std::cout << "GLFW error " << error << ": " << description << std::endl;
}

std::unordered_map<int, bool> g_isGlfwKeyDown;

void handleGlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		g_isGlfwKeyDown[key] = true;
	}

	if (action == GLFW_RELEASE)
	{
		g_isGlfwKeyDown[key] = false;
	}

	// We mark the window that it should close if ESC is pressed:
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(glfw_window, true);
	}
}

bool isKeyDown(int glfw_key_code)
{
	return g_isGlfwKeyDown[glfw_key_code];
}

std::vector<const char *> getRequiredInstanceExtensions()
{
	// Get extensions which GLFW requires:
	uint32_t num_glfw_extensions;
	const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);

	// Get extensions which Vulkan Launchpad requires:
	uint32_t num_vkl_extensions;
	const char **vkl_extensions = vklGetRequiredInstanceExtensions(&num_vkl_extensions);

	// Merge both arrays of extensions:
	std::vector<const char *> all_required_extensions(glfw_extensions, glfw_extensions + num_glfw_extensions);
	all_required_extensions.insert(all_required_extensions.end(), vkl_extensions, vkl_extensions + num_vkl_extensions);

	// Perform a sanity check if all the extensions are really supported by Vulkan on
	// this system (if they are not, we have a problem):
	for (auto ext : all_required_extensions)
	{
		if (!hlpIsInstanceExtensionSupported(ext))
		{
			VKL_EXIT_WITH_ERROR("Required extension \"" << ext << "\" is not supported");
		}
		VKL_LOG("Extension \"" << ext << "\" is supported");
	}

	return all_required_extensions;
}

uint32_t selectQueueFamilyIndex(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	// Get the number of different queue families for the given physical device:
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

	// Get the queue families' data:
	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	// Find a suitable queue family index and return it
	for (uint32_t i = 0; i < queue_family_count; ++i)
	{
		// Check if the queue family supports graphics operations
		bool supports_graphics = queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		// Check if the queue family supports presentation to the given surface
		VkBool32 supports_presentation = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &supports_presentation);

		// If the queue family supports both graphics and presentation, return its index
		if (supports_graphics && supports_presentation)
		{
			return i;
		}
	}

	VKL_EXIT_WITH_ERROR("Unable to find a suitable queue family that supports graphics and presentation on the same queue.");
}