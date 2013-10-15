/*
The MIT License

Copyright (c) 2009 - 2013 Liam Devine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "oolua_push_pull.h"


//if called by Lua
//	luaL_error - as it throw an exception if Lua compiled as C++
//				- returns an error code to the caller if compiled as C

//if called from cpp
//	using exceptions
//	throw

//	storing last error
//		store error
//		return false

#include "oolua_storage.h"
#include "oolua_error.h"

namespace OOLUA
{
	namespace INTERNAL
	{

		void handle_cpp_pull_fail(lua_State* vm, char const * expected_type)
		{
#	if OOLUA_USE_EXCEPTIONS == 1
			std::string message(std::string("Stack type is not a ") + expected_type);
			std::string stackType = lua_gettop(vm) ? lua_typename(vm, lua_type(vm, -1) ) : "empty stack";
			message += std::string(", yet ") + stackType;
			throw OOLUA::Type_error(message);
#	elif OOLUA_STORE_LAST_ERROR == 1
			lua_pushfstring(vm, "Stack type is not a %s, yet %s"
							, expected_type
							, lua_gettop(vm) ? lua_typename(vm, lua_type(vm, -1) ) : "empty stack");
			OOLUA::INTERNAL::set_error_from_top_of_stack_and_pop_the_error(vm);
			return;
#	else
			(void)vm;
			(void)expected_type;
			return;
#	endif
		}

		bool cpp_runtime_type_check_of_top(lua_State* vm, int expected_lua_type, char const * expected_type)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
			if( lua_gettop(vm) == 0 || lua_type(vm, -1) != expected_lua_type )
			{
				handle_cpp_pull_fail(vm, expected_type);
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
				return false;//dependant on config if the flow can return through this path
#	endif
			}
#else
			(void)vm;
			(void)expected_lua_type;
			(void)expected_type;
#endif
			return true;
		}

		bool cpp_runtime_type_check_of_top(lua_State* vm, compare_lua_type_func_sig compareFunc, char const * expected_type)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED == 1
			if(!compareFunc(vm, -1))
			{
				handle_cpp_pull_fail(vm, expected_type);
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
				return false;//dependant on config if the flow can return through this path
#	endif
			}
#else
			(void)vm;
			(void)compareFunc;
			(void)expected_type;
#endif
			return true;
		}

	} // namespace INTERNAL //NOLINT

	bool push(lua_State* const vm, void * lightud)
	{
		lua_pushlightuserdata(vm, lightud);
		return true;
	}

	bool push(lua_State* const vm, bool const& value)
	{
		assert(vm);
		lua_pushboolean(vm, (value? 1 : 0));
		return true;
	}

	bool push(lua_State* const vm, std::string const& value)
	{
		assert(vm);
		lua_pushlstring(vm, value.data(), value.size() );
		return true;
	}

	bool push(lua_State* const vm, char const * const& value)
	{
		assert(vm && value);
		lua_pushstring(vm, value);
		return true;
	}

	bool push(lua_State* const vm, char * const& value)
	{
		assert(vm && value);
		lua_pushstring(vm, value);
		return true;
	}

	bool push(lua_State* const vm, double const& value)
	{
		assert(vm);
		lua_pushnumber(vm, value);
		return true;
	}

	bool push(lua_State* const vm, float const&  value)
	{
		assert(vm);
		lua_pushnumber(vm, value);
		return true;
	}

	bool push(lua_State* const vm, lua_CFunction const &  value)
	{
		assert(vm);
		lua_pushcclosure(vm, value, 0);
		return true;
	}

	bool push(lua_State* const vm, Table const &  value)
	{
		assert(vm);
		return value.push_on_stack(vm);
	}

	bool pull(lua_State* const vm, void*& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TLIGHTUSERDATA, "light userdata") ) return false;
		value = lua_touserdata(vm, -1);
		return true;
	}

	bool pull(lua_State* const vm, bool& value)
	{
		/*
		If it is allowed to pull a bool from an int, check for number instead of boolean
		if(! INTERNAL::cpp_runtime_type_check_of_top(s,lua_isnumber,"bool") ) return false;
		 */
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TBOOLEAN, "bool") ) return false;
		value = lua_toboolean(vm, -1) ? true : false;
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, std::string& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TSTRING, "string") ) return false;
		size_t len(0);
		char const* str = lua_tolstring(vm, -1, &len);
		value = std::string(str, len);
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, double& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TNUMBER, "double") ) return false;
		value = static_cast<double>(lua_tonumber(vm, -1));
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, float& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TNUMBER, "float") ) return false;
		value = static_cast<float>(lua_tonumber(vm, -1));
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, lua_CFunction& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, lua_iscfunction, "lua_CFunction") ) return false;
		value = lua_tocfunction(vm, -1);
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, Table& value)
	{
		return value.pull_from_stack(vm);
	}

	namespace INTERNAL
	{

		namespace LUA_CALLED
		{

			void get_class_type_error(lua_State* const vm, char const* expected_type)
			{
				luaL_error(vm, "%s %s %s", "tried to pull type"
							, expected_type
							, "which is not the type or a base of the type on the stack");
			}

			void get_error(lua_State* vm, int idx, char const* expected_type)
			{
				luaL_error(vm, "trying to pull %s when %s is on stack"
							, expected_type
							, lua_typename(vm, lua_type(vm, idx)) );
			}

			void get(lua_State* const vm, int idx, void*& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( lua_type(vm, idx) != LUA_TLIGHTUSERDATA ) get_error(vm, idx, "light userdata");
#endif
				value = lua_touserdata(vm, idx);
			}

			void get(lua_State* const vm, int idx, bool& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isboolean(vm, idx) ) get_error(vm, idx, "bool");
#endif
				value = lua_toboolean(vm, idx) ? true : false;
			}

			void get(lua_State* const vm, int idx, char const*& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( lua_type(vm, idx) != LUA_TSTRING ) get_error(vm, idx, "char const*");
#endif
				value = lua_tolstring(vm, idx, 0);
			}

			void get(lua_State* const vm, int idx, char *& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isstring(vm, idx) ) get_error(vm, idx, "char*");
#endif
				value = const_cast<char*>(lua_tolstring(vm, idx, 0));
			}

//#if OOLUA_STD_STRING_IS_INTEGRAL == 1
			void get(lua_State* const vm, int idx, std::string& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if(lua_type(vm, idx) != LUA_TSTRING ) get_error(vm, idx, "std::string");
#endif
				size_t len(0);
				char const* str = lua_tolstring(vm, idx, &len);
				value = std::string(str, len);
			}
//#endif

			void get(lua_State* const vm, int idx, double& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "double");
#endif
				value = static_cast<double>(lua_tonumber(vm, idx));
			}

			void get(lua_State* const vm, int idx, float& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "float");
#endif
				value = static_cast<float>(lua_tonumber(vm, idx));
			}

			void get(lua_State* const vm, int idx, lua_CFunction& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_iscfunction(vm, idx) ) get_error(vm, idx, "lua_CFunction");
#endif
				value = lua_tocfunction(vm, idx);
			}

			void get(lua_State* const vm, int idx, Table& value)
			{
				value.lua_get(vm, idx);
			}

		} // namespace LUA_CALLED //NOLINT
	} // namespace INTERNAL //NOLINT
} // namespace OOLUA
