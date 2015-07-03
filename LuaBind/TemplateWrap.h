
#pragma once
#include "lua.hpp"

#include <tuple>               
#include <type_traits>

//

template <typename T, typename Tuple>
struct has_type;

template <typename T>
struct has_type<T, std::tuple<>> : std::false_type{};

template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>>{};

template <typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type{};

//

template<class ...Args>
struct is_distinct;

template <>
struct is_distinct<> : std::true_type{};

template<class T, class ...Args>
struct is_distinct < T, Args... > : std::conditional<
    has_type<T, std::tuple<Args...>>::value,
    std::false_type, std::true_type>::type{};

//


template<class Signature>
struct num_arguments;

template<class T, class ...Signatures>
struct num_arguments < T(Signatures...) > {
    static const int value = sizeof...(Signatures);
};

//

void templatePush(lua_State *L);