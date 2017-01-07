/* Copyright (c) 2017 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"
#include "Lua/LuaEngine.h"
#include "Lua/ToLua/tolua++.h"
#include "Lua/ToLua/tolua_fix.h"

NS_DOROTHY_BEGIN

static int g_ref_id = 0;
static stack<int> g_available_ref_ids;

int tolua_get_max_callback_ref_count()
{
	return g_ref_id;
}

int tolua_get_callback_ref_count()
{
	return g_ref_id - (unsigned int)g_available_ref_ids.size();
}

int tolua_alloc_callback_ref_id()
{
	if (g_available_ref_ids.empty())
	{
		return ++g_ref_id;
	}
	else
	{
		int id = g_available_ref_ids.top();
		g_available_ref_ids.pop();
		return id;
	}
}

void tolua_collect_callback_ref_id(int refid)
{
	g_available_ref_ids.push(refid);
}

int tolua_collect_object(lua_State* L)
{
	Object* object = r_cast<Object*>(tolua_tousertype(L, 1, 0));
	object->removeLuaRef();
	object->release();
	return 0;
}

void tolua_pushobject(lua_State* L, Object* object)
{
	if (!object)
	{
		lua_pushnil(L);
		return;
	}
	int refid = object->getLuaRef();

	lua_rawgeti(L, LUA_REGISTRYINDEX, TOLUA_REG_INDEX_UBOX); // ubox
	lua_rawgeti(L, -1, refid); // ubox ud

	if (lua_isnil(L, -1)) // ud == nil
	{
		lua_pop(L, 1); // ubox
		*r_cast<void**>(lua_newuserdata(L, sizeof(void*))) = object; // ubox newud
		lua_pushvalue(L, -1); // ubox newud newud
		lua_insert(L, -3); // newud ubox newud
		lua_rawseti(L, -2, refid); // ubox[refid] = newud, newud ubox
		lua_pop(L, 1); // newud
		lua_rawgeti(L, LUA_REGISTRYINDEX, object->getDoraType()); // newud mt
		lua_setmetatable(L, -2); // newud<mt>, newud
		lua_pushvalue(L, TOLUA_NOPEER);
		lua_setfenv(L, -2);
		// register oObject GC
		object->addLuaRef();
		object->retain();
	}
	else lua_remove(L, -2);// ud
}

void tolua_pushslice(lua_State* L, String str)
{
	lua_pushlstring(L, str.rawData(), str.size());
}

void tolua_typeid(lua_State* L, int typeId, const char* className)
{
	lua_getfield(L, LUA_REGISTRYINDEX, className); // mt
	lua_rawseti(L, LUA_REGISTRYINDEX, typeId); // empty
}

int tolua_isobject(lua_State* L, int lo)
{
	if (lua_isuserdata(L, lo))
	{
		lua_getmetatable(L, lo); // mt
		lua_rawgeti(L, -1, MT_SUPER); // mt tb
		lua_rawgeti(L, LUA_REGISTRYINDEX, LuaType<Object>()); // mt tb ccobjmt
		lua_rawget(L, LUA_REGISTRYINDEX); // mt tb "Object"
		lua_rawget(L, -2); // tb["oObject"], mt tb flag
		int result = lua_toboolean(L, -1);
		lua_pop(L, 3); // empty
		return result;
	}
	return 0;
}

void tolua_dobuffer(lua_State* L, char* codes, unsigned int size, const char* name)
{
	if (luaL_loadbuffer(L, codes, size, name) != 0)
	{
		Log("error loading module %s from %s :\n\t%s",
			lua_tostring(L, 1), name, lua_tostring(L, -1));
	}
	else LuaEngine::call(L, 0, 0);
}

int tolua_ref_function(lua_State* L, int lo)
{
	/* function at lo */
    if (!lua_isfunction(L, lo)) return 0;
	int refid = tolua_alloc_callback_ref_id();
	lua_rawgeti(L, LUA_REGISTRYINDEX, TOLUA_REG_INDEX_CALLBACK); // funcMap
    lua_pushvalue(L, lo); // funcMap fun
    lua_rawseti(L, -2, refid); // funcMap[refid] = fun, funcMap
	lua_pop(L, 1); // empty
    return refid;
}

void tolua_get_function_by_refid(lua_State* L, int refid)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, TOLUA_REG_INDEX_CALLBACK); // funcMap
    lua_rawgeti(L, -1, refid); // funcMap fun
    lua_remove(L, -2); // fun
}

void tolua_remove_function_by_refid(lua_State* L, int refid)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, TOLUA_REG_INDEX_CALLBACK); // funcMap
	lua_pushnil(L); // funcMap nil
	lua_rawseti(L, -2, refid); // funcMap[refid] = nil, funcMap
	lua_pop(L, 1); // empty
	tolua_collect_callback_ref_id(refid);
}

int tolua_isfunction(lua_State* L, int lo, tolua_Error* err)
{
    if (lua_gettop(L) >= abs(lo) && lua_isfunction(L, lo))
    {
        return 1;
    }
    err->index = lo;
    err->array = 0;
    err->type = "function";
    return 0;
}

void tolua_stack_dump(lua_State* L, int offset, const char* label)
{
    int top = lua_gettop(L) + offset;
	if (top == 0)
	{
		return;
	}
    Print("Total [%d] in lua stack: %s\n", top, label != 0 ? label : "");
    for (int i = -1; i >= -top; i--)
    {
        int t = lua_type(L, i);
        switch (t)
        {
            case LUA_TSTRING:
                Print("  [%02d] [string] %s\n", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                Print("  [%02d] [boolean] %s\n", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                Print("  [%02d] [number] %g\n", i, lua_tonumber(L, i));
                break;
            default:
                Print("  [%02d] %s\n", i, lua_typename(L, t));
        }
    }
}

NS_DOROTHY_END
