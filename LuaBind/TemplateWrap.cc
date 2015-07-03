
#include "TemplateWrap.h"
#include "lua.hpp"

//

template <typename T, typename Tuple>
struct combine_tuples;

template <typename T, typename ...Signatures>
struct combine_tuples < T, std::tuple<Signatures...> > {
    using type = std::tuple < T, Signatures... > ;
};

//

template <typename NewRow, int row_id, typename Original>
struct replace_row {
    using type = std::tuple < NewRow >;
};


//template <typename T, typename ...Signatures>
//struct combine_tuples < T, std::tuple<Signatures...> > {
//    using type = std::tuple < T, Signatures... >;
//};

//

template<typename With, int num, typename SmallTuple>
struct expand_to {
    using type = SmallTuple;
};

//

template<typename ...Signatures>
struct FullMetadataGenerator;

template<>
struct FullMetadataGenerator < > {
    using type = std::tuple < > ;
};

template<typename Sig, typename ...Signatures>
struct FullMetadataGenerator < Sig, Signatures... > {
//    // tuple position -> num_arguments<Sig>::value;
//    // combine with FullMetadataGenerator<Signatures...>::type;
//
//private:
//    static const int num_args = num_arguments<Sig>::value;
//    using previous_expanded = expand_to<std::tuple<>, num_args, FullMetadataGenerator<Signatures...>::type>::type;
//
//    using previous_row = std::tuple_element<num_args, previous_expanded>::type;
//    using new_row = combine_tuples<Sig, previous_row>::type;
//
//public:
//
//    using type = combine_tuples <
//        num_args,
//        Sig,
//        previous_expanded
//    > ::type;
};

template<>
struct FullMetadataGenerator<void(*)(int), void(*)(const char*)> {
    using type = std::tuple<
        std::tuple<>, // 0 args
        std::tuple<void(int), void(const char*)>  // 1 arg
    >;
};

template<class FullMetadata, int Pos>
int recursive_find_overload_function_index(int n, int current_index) {
    if (n == Pos) {
        return current_index;
    } else {
        int is_current_index_nonempty = std::tuple_size<
            std::tuple_element<Pos, FullMetadata>::type
        >::value != 0;
        return recursive_find_overload_function_index<FullMetadata, Pos + 1>(n, current_index + is_current_index_nonempty);
    }
}

template<class FullMetadata>
int find_overload_function_index_for_n_arguments(int n) {
    int function_receives_at_most = std::tuple_size<FullMetadata>::value - 1;
    if (n > function_receives_at_most)
        return -1;
    return recursive_find_overload_function_index<FullMetadata, 0>(n);
}

template<class FullMetadata>
int multiple_argument_count_wrapper(lua_State *L) {
    int num_arguments = lua_gettop(L);
    int index = find_overload_function_index_for_n_arguments<FullMetadata>(num_arguments);

    if (index < 0) {
        return luaL_error(L, "no overload that receives %d arguments", num_arguments);
    }
    lua_CFunction* f = (lua_CFunction*)lua_touserdata(L, lua_upvalueindex(index + 1));
    return f(L);
}

template<class ...Signatures>
void push_overloaded_function(lua_State *L, const char* name, Signatures... args) {
    static_assert(is_distinct<Signatures...>::value, "Can't overload two functions with the same signature");
    int c = sizeof...(Signatures);
    
    //using FullMetadata = FullMetadataGenerator < Signatures... >::type;
    //FullMetadata m;
    //auto x = std::get<1>(f);

    //auto f = multiple_argument_count_wrapper < FullMetadata >;
}


void foo(int) {
    puts("foo (void(int)");
}

void foo(const char*) {
    puts("foo (void(const char*)");
}

void foo(int, double) {
    puts("foo (void(int, double)");
}

void foo(const char*, double) {
    puts("foo (void(const char*, double)");
}

void templatePush(lua_State *L) {
    push_overloaded_function(L, "foo",
                             (void(*)(int)) &foo,
                             (void(*)(const char*)) &foo,
                             (void(*)(int, double)) &foo,
                             (void(*)(const char*, double)) &foo
                             );

    static_assert(num_arguments<int(void)>::value == 0, "budega");

    replace_row<int, 1, std::tuple<double, void, char>>::type x;    
}