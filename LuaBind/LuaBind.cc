// LuaBind.cc : Defines the exported functions for the DLL application.
//

#include "lua.hpp"
#include <cstdio>

static int call_bound(lua_State *L) {
    int num_args = lua_gettop(L);
    int num_upvalues = luaL_checkinteger(L, lua_upvalueindex(1));
    printf("ESTAMOS SENDO CHAMADOS WITH %d VALUES\n", num_upvalues);

    luaL_checkstack(L, num_upvalues, "stack maximum size reaching when pushing bound values");
    //int previous_top = lua_gettop(L);
    for (int i = 0; i < num_upvalues; ++i) {
        lua_pushvalue(L, lua_upvalueindex(i + 2));
    }
    for (int i = 0; i < num_args; ++i) {
        lua_pushvalue(L, i + 1);
    }
    lua_call(L, num_args + num_upvalues - 1, 0);
    return 0;
    //lua_call(L, num_upvalues - 1, LUA_MULTRET);
    //return lua_gettop(L) - previous_top;
}

static int bind(lua_State *L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    int num_upvalues = lua_gettop(L);
    lua_pushinteger(L, num_upvalues);
    lua_insert(L, 1);
    lua_pushcclosure(L, call_bound, lua_gettop(L));
    return 1;
}

static const struct luaL_reg bindmodule_reg[] = {
    { "bind", bind },
    { NULL, NULL }  /* sentinel */
};

extern "C"
__declspec(dllexport) int luaopen_bind(lua_State *L) {
    luaL_openlib(L, "bind", bindmodule_reg, 0);
    templatePush(L);
    return 1;
}
