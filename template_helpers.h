#pragma once

#include <memory>
#include <tuple>

namespace template_helpers {

    //
    // Extract type T from std::shared_ptr<T>
    //

    template<typename T>
    struct remove_shared_ptr
    {
        using type = T;
    };

    template<typename T>
    struct remove_shared_ptr<std::shared_ptr<T>>
    {
        using type = T;
    };

    template<typename T>
    struct remove_shared_ptr<const std::shared_ptr<T>&> : public remove_shared_ptr<std::shared_ptr<T>> {};

    template<typename T>
    using remove_shared_ptr_t = typename remove_shared_ptr<T>::type;

    //
    // Parameter validation (compile time)
    //

    template <typename TBase, typename... Rest>
    struct validate_arg;

    template <typename TBase, typename T>
    struct validate_arg<TBase, T>
    {
        static_assert(
            std::is_base_of_v<TBase, remove_shared_ptr_t<T>>,
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
    constexpr auto validate_arg_tuple_impl(Tuple&& t, std::index_sequence<I...>)
    {
        validate_arg<TBase, std::get<I>(std::forward<Tuple>(t))...> validate;
    }

    template <typename TBase, typename Tuple>
    constexpr auto validate_arg_tuple()
    {
        validate_arg<TBase, Tuple>{};
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
    // Build tuple of shared_ptr's from a tuple of IIDs (run time)
    // (requires the usage of an external resolver at runtime)
    //

    template <typename TypeTuple, typename Resolver, typename IIDTuple, std::size_t... I>
    auto interface_to_service_tuple_impl(const Resolver& resolver, IIDTuple&& it, std::index_sequence<I...>)
    {
        return std::make_tuple(
            static_pointer_cast<remove_shared_ptr_t<std::tuple_element_t<I, TypeTuple>>>(
                resolver(std::get<I>(std::forward<IIDTuple>(it))))...);
    }

    template<typename TypeTuple, typename Resolver, typename IIDTuple>
    auto interface_to_service_tuple(Resolver&& resolver, IIDTuple&& it)
    {
        constexpr auto size = std::tuple_size<std::decay_t<IIDTuple>>::value;
        return interface_to_service_tuple_impl<TypeTuple>(
            std::forward<Resolver>(resolver), 
            std::forward<IIDTuple>(it), 
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
