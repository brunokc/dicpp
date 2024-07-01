#pragma once

#include <tuple>

template<typename T>
class function_traits
{
    static_assert(sizeof(T) == 0, "function_traits<T>: T is not a function type");
};

template<typename R, typename... Args>
struct function_traits<R(Args...)>
{
    constexpr static const size_t arity = sizeof...(Args);
    using result_type = R;

    using args_tuple = std::tuple<Args...>;

    //template<size_t i>
    //struct arg
    //{
    //    using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    //};

    //template<size_t i>
    //using arg_type = typename std::tuple_element<i, std::tuple<Args...>>::type;
};

template<typename R, typename... Args>
struct function_traits<R(Args...) const> : function_traits<R(Args...)> {};

template<typename R, typename... Args>
struct function_traits<R(Args...)&> : function_traits<R(Args...)> {};

template<typename R, typename... Args>
struct function_traits<R(Args...) const&> : function_traits<R(Args...)> {};

template<typename R, typename... Args>
struct function_traits<R(Args...)&&> : function_traits<R(Args...)> {};

template<typename R, typename... Args>
struct function_traits<R(Args...) const&&> : function_traits<R(Args...)> {};

// Member functions

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)&> : function_traits<R(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const&> : function_traits<R(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)&&> : function_traits<R(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const&&> : function_traits<R(Args...)> {};
