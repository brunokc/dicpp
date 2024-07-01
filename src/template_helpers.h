#pragma once

#include <memory>
#include <tuple>
#include <typeindex>
#include <type_traits>

namespace template_helpers {

    //
    // Extract type T from std::shared_ptr<T>
    //

    template<typename T>
    struct extract_ptr_type
    {
        using type = T;
    };

    template<typename T>
    struct extract_ptr_type<T*>
    {
        using type = T;
    };

    template<typename T>
    struct extract_ptr_type<std::shared_ptr<T>>
    {
        using type = T;
    };

    template<typename T>
    struct extract_ptr_type<const std::shared_ptr<T>&> : public extract_ptr_type<std::shared_ptr<T>> {};

    template<typename T>
    using extract_ptr_type_t = typename extract_ptr_type<T>::type;

    //
    // Parameter validation (compile time)
    //

    template <typename TBase, typename... Rest>
    struct validate_arg;

    template <typename TBase, typename T>
    struct validate_arg<TBase, T>
    {
        static_assert(
            std::is_base_of_v<TBase, extract_ptr_type_t<T>>,
            "Arguments to service Create() method must derive from IService");
    };

    template <typename TBase, typename T, typename... Rest>
    struct validate_arg<TBase, T, Rest...>
    {
        validate_arg<TBase, T> validate_first;
        validate_arg<TBase, Rest...> validate_rest;
    };

    template <typename TBase, typename... Rest>
    struct validate_arg<TBase, std::tuple<Rest...>>
    {
        validate_arg<TBase, Rest...> validate_rest;
    };

    template <typename TBase, typename Tuple, std::size_t... I>
    constexpr auto validate_arg_tuple_impl(std::index_sequence<I...>)
    {
        validate_arg<TBase, std::tuple_element_t<I, Tuple>...> validate;
    }

    template <typename TBase, typename Tuple>
    constexpr auto validate_arg_tuple()
    {
        validate_arg<TBase, Tuple>{};
    }

    //
    // Build tuple of raw interface pointers types (IFoo*) from tuple of interface types (compile time)
    //

    template <typename Tuple, std::size_t... I>
    constexpr auto extract_smart_ptr_tuple_impl(std::index_sequence<I...>)
    {
        return std::tuple<std::add_pointer_t<extract_ptr_type_t<std::tuple_element_t<I, Tuple>>>...>{};
    }

    template<typename Tuple>
    constexpr auto extract_smart_ptr_tuple()
    {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return extract_smart_ptr_tuple_impl<Tuple>(std::make_index_sequence<size>{});
    }

    //
    // Build tuple of IIDs from tuple of interface types (compile time)
    //

    template <typename Tuple, std::size_t... I>
    constexpr auto interface_iid_tuple_impl(std::index_sequence<I...>)
    {
        return std::make_tuple(__uuidof(std::tuple_element_t<I, Tuple>)...);
    }

    template<typename Tuple>
    constexpr auto interface_to_iid_tuple()
    {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return interface_iid_tuple_impl<Tuple>(std::make_index_sequence<size>{});
    }

    //
    // Build tuple of type indexes from tuple of interface types (run time)
    //

    template <typename Tuple, std::size_t... I>
    auto interface_to_indexes_tuple_impl(std::index_sequence<I...>)
    {
        return std::make_tuple(std::type_index(typeid(extract_ptr_type_t<std::tuple_element_t<I, Tuple>>))...);
    }

    template<typename Tuple>
    auto interface_to_indexes_tuple()
    {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return interface_to_indexes_tuple_impl<Tuple>(std::make_index_sequence<size>{});
    }

    //
    // Build tuple of shared_ptr's from a tuple of IIDs (run time)
    // (requires the usage of an external resolver at runtime)
    //

    template <typename TypeTuple, typename Resolver, typename IdxTuple, std::size_t... I>
    auto interface_to_service_tuple_impl(const Resolver& resolver, IdxTuple&& it, std::index_sequence<I...>)
    {
        return std::make_tuple(
            static_pointer_cast<extract_ptr_type_t<std::tuple_element_t<I, TypeTuple>>>(
                resolver(std::get<I>(std::forward<IdxTuple>(it))))...);
    }

    template<typename TypeTuple, typename Resolver, typename IdxTuple>
    auto interface_to_service_tuple(Resolver&& resolver, IdxTuple&& it)
    {
        constexpr auto size = std::tuple_size<std::decay_t<IdxTuple>>::value;
        return interface_to_service_tuple_impl<TypeTuple>(
            std::forward<Resolver>(resolver), 
            std::forward<IdxTuple>(it),
            std::make_index_sequence<size>{});
    }

    //
    // Invoke a function passing the contents of a tuple as arguments (run time)
    //

    template <typename Func, typename Tuple, std::size_t... I>
    decltype(auto) invoke_function(Func&& func, Tuple&& t, std::index_sequence<I...>) 
    {
        return func(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename Func, typename Tuple>
    decltype(auto) invoke_with_tuple(Func&& func, Tuple&& t) 
    {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return invoke_function(std::forward<Func>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }

} // namespace template_helpers
