#include "VulkanValidator.h"

namespace mvk {
    void VulkanValidator::CheckRequestedExtensions(std::vector<const char *> requiement_extensions) {
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

    std::vector<const char*> VulkanValidator::GetRequirmentInstanceExtension(bool enable_validation_layers) {
        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            extensions.push_back("VK_KHR_get_physical_device_properties2");
        }

        return extensions;
    }

    bool VulkanValidator::CheckValidationLayersSupport(std::vector<const char *> validation_layers) {
        auto layers = vk::enumerateInstanceLayerProperties();

        for (auto &requested_layer : validation_layers) {
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

    bool VulkanValidator::CheckVideocard(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> device_required_ext) {
        QueueFamilies family = QueueFamilies::FindQueueFamily(device, surface);
        bool ext_check = CheckDeviceExtensions(device, device_required_ext);

        bool swap_chain_support = false;
        if (ext_check) {
            SwapChainDetails sc(device, surface);
            swap_chain_support = !sc.format_.empty() && !sc.present_modes_.empty();
        }

        return family.IsComplete() && ext_check;
    }

    bool VulkanValidator::CheckDeviceExtensions(vk::PhysicalDevice device, std::vector<const char *> device_required_ext) {
        auto device_exts = device.enumerateDeviceExtensionProperties();

        size_t matches = 0;
        for (auto &requested_ext : device_required_ext) {
            for (auto &ext : device_exts) {
                if (!strcmp(ext.extensionName, requested_ext)) {
                    matches++;
                }
            }
        }
        if (matches != device_required_ext.size())
            return false;

        return true;
    }
}
