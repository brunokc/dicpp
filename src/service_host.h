#pragma once

#include <vector>
#include <memory>

#include <Windows.h>

#include "function_traits.h"
#include "template_helpers.h"

#pragma push_macro("CreateService")
#undef CreateService

// Common base class for all registered services
struct IService { };

template<typename T>
struct what;

class ServiceHost
{
private:
    struct ServiceEntry
    {
        std::type_index index;
        std::shared_ptr<IService> service;
    };

public:
    template<class T>
    void RegisterService()
    {
        using arg_type_tuple = function_traits<decltype(T::Create)>::args_tuple;

        // Validate Create() arguments are all interface pointers that are all derived from IService
        template_helpers::validate_arg_tuple<IService, arg_type_tuple>();

        auto interface_arg_type_tuple = template_helpers::extract_smart_ptr_tuple<arg_type_tuple>();

        // Validate Create() arguments are all interface pointers that are all derived from IService
        template_helpers::validate_arg_tuple<IService, decltype(interface_arg_type_tuple)>();

        // Extract a tuple of IIDs from the tuple of interface pointers
        auto indexes = template_helpers::interface_to_indexes_tuple<decltype(interface_arg_type_tuple)>();

        // Generate tuple of service instances from the interfaces listed in the Create() parameters
        auto resolver = [this](const std::type_index& index)
            {
                return GetService<IService>(index);
            };
        auto params = template_helpers::interface_to_service_tuple<decltype(interface_arg_type_tuple)>(resolver, indexes);

        // Invoke Create() method passing argument tuple
        auto service = template_helpers::invoke_with_tuple(&T::Create, params);
        _registeredServices.emplace_back(ServiceEntry{ std::type_index(typeid(T)), std::move(service) });
    }

    template<class I, class T>
    void RegisterService()
    {
        _registeredServices.emplace_back(ServiceEntry{ std::type_index(typeid(I)), std::make_shared<T>() });
    }

    template<class I>
    std::shared_ptr<I> GetService()
    {
        return GetService<I>(std::type_index(typeid(I)));
    }

private:
    template<typename I>
    std::shared_ptr<I> GetService(const std::type_index& index)
    {
        for (auto&& entry : _registeredServices)
        {
            if (entry.index == index)
            {
                return std::static_pointer_cast<I>(entry.service);
            }
        }
        return nullptr;
    }

private:
    std::vector<ServiceEntry> _registeredServices;
};

#pragma pop_macro("CreateService")
