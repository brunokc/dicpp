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

class ServiceHost
{
private:
    struct ServiceEntry
    {
        IID iid;
        std::shared_ptr<IService> service;
    };

public:
    template<class T>
    void RegisterService()
    {
        using arg_type_tuple = function_traits<decltype(T::Create)>::args_tuple;

        // Validate Create() arguments are all interface pointers that are all derived from IService
        template_helpers::validate_arg_tuple<IService, arg_type_tuple>();

        // Extract a tuple of IIDs from the tuple of interface pointers
        auto iids = template_helpers::interface_to_iid_tuple<arg_type_tuple>();

        // Generate tuple of service instances from the interfaces listed in the Create() parameters
        auto resolver = [this](REFIID iid)
            {
                return GetService<IService>(iid);
            };
        auto params = template_helpers::interface_to_service_tuple<arg_type_tuple>(resolver, iids);

        // Invoke Create() method passing argument tuple
        auto service = template_helpers::invoke_with_tuple(&T::Create, params);
        _registeredServices.emplace_back(ServiceEntry{ __uuidof(T), std::move(service) });
    }

    template<class I, class T>
    void RegisterService()
    {
        _registeredServices.emplace_back(ServiceEntry{ __uuidof(I), std::make_shared<T>() });
    }

    template<class I>
    std::shared_ptr<I> GetService()
    {
        return GetService<I>(__uuidof(I));
    }

private:
    template<typename I>
    std::shared_ptr<I> GetService(REFIID iid)
    {
        for (auto&& entry : _registeredServices)
        {
            if (entry.iid == iid)
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
