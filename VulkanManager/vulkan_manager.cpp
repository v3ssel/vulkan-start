#include "../VulkanManager/vulkan_manager.h"
#include "queue_families.h"
#include "vulkan_manager.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {

    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cout << "\u001b[31mERROR: " << callback_data->pMessage << '\n';
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cout << "\u001b[33mWARNING: " << callback_data->pMessage << '\n';
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        std::cout << "\u001b[32mINFO: " << callback_data->pMessage << '\n';
    else
        std::cout << "\u001b[0mVERBOSE: " << callback_data->pMessage << '\n';
    
    return VK_FALSE;
}

namespace mvk {
    void VulkanManager::CreateInstance() {
        if (enableValidationLayers && !CheckValidationLayersSupport())
            throw std::runtime_error("Requested validation layers unavailable.");

        vk::ApplicationInfo app_info;
        app_info.sType = vk::StructureType::eApplicationInfo;
        app_info.setPApplicationName("Vulkan Triangle");
        app_info.setApplicationVersion(VK_MAKE_API_VERSION(0, 1, 0, 0));
        app_info.setPEngineName("No Engine");
        app_info.setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0));
        app_info.setApiVersion(VK_API_VERSION_1_0);

        vk::InstanceCreateInfo create_info{};
        create_info.sType = vk::StructureType::eInstanceCreateInfo;
        create_info.setPApplicationInfo(&app_info);

        auto requirment_extensions = GetRequirmentExtension();
        this->CheckRequestedExtensions(requirment_extensions);
        create_info.setEnabledExtensionCount(requirment_extensions.size());
        create_info.setPpEnabledExtensionNames(requirment_extensions.data());

        vk::DebugUtilsMessengerCreateInfoEXT debug_info{};
        if (enableValidationLayers) {
            create_info.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
            create_info.setPpEnabledLayerNames(validationLayers.data());

            FillDebugInfo(debug_info);
            create_info.setPNext((vk::DebugUtilsMessengerCreateInfoEXT *)&debug_info);
        } else {
            create_info.setEnabledLayerCount(0);
            create_info.setPNext(nullptr);
        }

        this->PrintLoadedData();

        if (vk::createInstance(&create_info, nullptr, &instance_) != vk::Result::eSuccess) {
            throw std::runtime_error("Cannot create instance.");
        }
    }

    void VulkanManager::SetupDebug() {
        if (!enableValidationLayers) return;

        vk::DebugUtilsMessengerCreateInfoEXT debug_info;
        FillDebugInfo(debug_info);

        debug_messenger_ = instance_.createDebugUtilsMessengerEXT(debug_info, nullptr, vk::DispatchLoaderDynamic(instance_, vkGetInstanceProcAddr));
    }

    void VulkanManager::TakeVideocard() {
        auto devices = instance_.enumeratePhysicalDevices();

        if (devices.size() == 0) throw std::runtime_error("Supported GPU not found.");

        for (auto &device : devices) {
            if (CheckVideocard(device)) {
                physical_device_ = device;
                break;
            }
        }

        if (&physical_device_ == VK_NULL_HANDLE)
            throw std::runtime_error("Supported GPU not found.");
    }

    void VulkanManager::CreateLogicalDevice() {
        QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamily(physical_device_);
        vk::DeviceQueueCreateInfo logical_device_queue_info{};
        logical_device_queue_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
        logical_device_queue_info.setQueueFamilyIndex(indices.family_.value());
        logical_device_queue_info.setQueueCount(1);
        float q = 1.0f;
        logical_device_queue_info.setQueuePriorities(q);

        vk::DeviceCreateInfo logical_device_info{};
        logical_device_info.sType = vk::StructureType::eDeviceCreateInfo;
        logical_device_info.setPQueueCreateInfos(&logical_device_queue_info);
        logical_device_info.setQueueCreateInfoCount(1);

        vk::PhysicalDeviceFeatures features{};
        logical_device_info.setPEnabledFeatures(&features);

        logical_device_ = physical_device_.createDevice(logical_device_info);
        graphics_queue_ = logical_device_.getQueue(indices.family_.value(), 0);
    }
    
    void VulkanManager::CheckRequestedExtensions(std::vector<const char *> requiement_extensions) {
        auto extensions = vk::enumerateInstanceExtensionProperties();
        uint32_t match_count = 0;

        for (size_t i = 0; i < requiement_extensions.size(); ++i) {
            for (size_t j = 0; j < extensions.size(); ++j) {
                if (!strcmp(extensions[j].extensionName, requiement_extensions[i])) {
                    match_count++;
                }
            }
        }
        
        if (match_count != requiement_extensions.size())
            throw std::runtime_error("Cannot find GLFWExtensions in InstanceExtentions.");
    }

    bool VulkanManager::CheckValidationLayersSupport() {
        auto layers = vk::enumerateInstanceLayerProperties();

        for (auto &requested_layer : validationLayers) {
            bool layer_found = false;
            for (auto &layer : layers) {
                if (!strcmp(requested_layer, layer.layerName)) {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
                return false;
        }

        return true;
    }

    std::vector<const char*> VulkanManager::GetRequirmentExtension() {
        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }
 
    void VulkanManager::FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT& debug_info) {
        debug_info.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        debug_info.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        );

        debug_info.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        );

        debug_info.setPfnUserCallback(DebugCallback);
        debug_info.setPUserData(nullptr);
    }

    bool VulkanManager::CheckVideocard(vk::PhysicalDevice device) {
        QueueFamilyIndices family = QueueFamilyIndices::FindQueueFamily(device);
        return family.IsComplete();
    }

    void VulkanManager::PrintLoadedData() {
        auto extensions = vk::enumerateInstanceExtensionProperties();
        std::cout << "LOADED EXTENSIONS:\n";
        for (auto &ext : extensions)
            std::cout << '\t' << ext.extensionName << '\n';


        auto layers = vk::enumerateInstanceLayerProperties();
        std::cout << "LAYERS:\n";
        for (auto &layer : layers)
            std::cout << '\t' << layer.layerName << "\n";

    }
}