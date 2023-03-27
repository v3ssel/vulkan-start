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
        if (ENABLE_VALIDATION_LAYERS && !validator_.CheckValidationLayersSupport(VALIDATION_LAYERS))
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

        auto requirment_extensions = validator_.GetRequirmentInstanceExtension(ENABLE_VALIDATION_LAYERS);
        validator_.CheckRequestedExtensions(requirment_extensions);
        create_info.setEnabledExtensionCount(requirment_extensions.size());
        create_info.setPpEnabledExtensionNames(requirment_extensions.data());

        vk::DebugUtilsMessengerCreateInfoEXT debug_info{};
        if (ENABLE_VALIDATION_LAYERS) {
            create_info.setEnabledLayerCount(static_cast<uint32_t>(VALIDATION_LAYERS.size()));
            create_info.setPpEnabledLayerNames(VALIDATION_LAYERS.data());

            FillDebugInfo(debug_info);
            create_info.setPNext((vk::DebugUtilsMessengerCreateInfoEXT *)&debug_info);
        } else {
            create_info.setEnabledLayerCount(0);
            create_info.setPNext(nullptr);
        }

        if (vk::createInstance(&create_info, nullptr, &instance_) != vk::Result::eSuccess) {
            throw std::runtime_error("Cannot create instance.");
        }
    }

    void VulkanManager::SetupDebug() {
        if (!ENABLE_VALIDATION_LAYERS) return;

        vk::DebugUtilsMessengerCreateInfoEXT debug_info;
        FillDebugInfo(debug_info);

        debug_messenger_ = instance_.createDebugUtilsMessengerEXT(debug_info, nullptr, vk::DispatchLoaderDynamic(instance_, vkGetInstanceProcAddr));
    }

    void VulkanManager::TakeVideocard() {
        auto devices = instance_.enumeratePhysicalDevices();

        if (devices.size() == 0) throw std::runtime_error("Supported GPU not found.");

        for (auto &device : devices) {
            if (validator_.CheckVideocard(device, surface_, DEVICE_REQUIRED_EXTENSIONS)) {
                physical_device_ = device;
                break;
            }
        }
        if (static_cast<VkPhysicalDevice>(physical_device_) == nullptr)
            throw std::runtime_error("Supported GPU not found.");
    }

    void VulkanManager::CreateLogicalDevice() {
        QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamily(physical_device_, surface_);

        std::vector<vk::DeviceQueueCreateInfo> device_queue_infos{};
        std::set<uint32_t> unique_families = {indices.graphics_family_.value(), indices.present_family_.value()};

        float q = 1.0f;
        for (auto &queue_family : unique_families) {
            vk::DeviceQueueCreateInfo logical_device_queue_info{};
            logical_device_queue_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
            logical_device_queue_info.setQueueFamilyIndex(indices.graphics_family_.value());
            logical_device_queue_info.setQueueCount(1);
            logical_device_queue_info.setQueuePriorities(q);
            device_queue_infos.push_back(logical_device_queue_info);
        }

        vk::DeviceCreateInfo logical_device_info{};
        logical_device_info.sType = vk::StructureType::eDeviceCreateInfo;
        logical_device_info.setQueueCreateInfoCount(device_queue_infos.size());
        logical_device_info.setPQueueCreateInfos(device_queue_infos.data());
        logical_device_info.setEnabledExtensionCount(static_cast<uint32_t>(DEVICE_REQUIRED_EXTENSIONS.size()));
        logical_device_info.setPpEnabledExtensionNames(DEVICE_REQUIRED_EXTENSIONS.data());

        vk::PhysicalDeviceFeatures features{};
        logical_device_info.setPEnabledFeatures(&features);

        logical_device_ = physical_device_.createDevice(logical_device_info);
        graphics_queue_ = logical_device_.getQueue(indices.graphics_family_.value(), 0);
        present_queue_ = logical_device_.getQueue(indices.present_family_.value(), 0);
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

    void VulkanManager::PrintLoadedData() {
        auto extensions = vk::enumerateInstanceExtensionProperties();
        std::cout << "\u001b[36mLOADED EXTENSIONS:\n";
        for (auto &ext : extensions)
            std::cout << '\t' << ext.extensionName << '\n';


        auto layers = vk::enumerateInstanceLayerProperties();
        std::cout << "LAYERS:\n";
        for (auto &layer : layers)
            std::cout << '\t' << layer.layerName << "\n";

        auto devices = instance_.enumeratePhysicalDevices();
        std::cout << "DEVICES:\n";
        for (auto &device : devices) {
            std::cout << "\t" << device.getProperties().deviceName << "\n\tEXTENSIONS:\n";
            // for (auto& device_ext : device.enumerateDeviceExtensionProperties())
            //     std::cout << "\t\t" << device_ext.extensionName << "\n";

            // std::cout << "\t" << "LAYERS:\n";
            // for (auto& layer : device.enumerateDeviceLayerProperties())
            //     std::cout << "\t\t" << layer.layerName << "\n";

            // std::cout << "\t" << "QUEUES:\n";
            // for (auto &queue : device.getQueueFamilyProperties())
            //     std::cout << "\t\t" << queue.queueCount << "\n";
        }
        std::cout << "\u001b[0m";
    }
}
