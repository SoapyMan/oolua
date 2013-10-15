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

/**
	\file class_public_member.h
	\brief Proxies a class public member variable
*/

#ifndef CLASS_PUBLIC_MEMBER_H_
#	define CLASS_PUBLIC_MEMBER_H_

#include "fwd_push_pull.h"
#include "param_traits.h"

namespace OOLUA
{
	/** \cond INTERNAL */
	namespace INTERNAL
	{

		template<typename T, int shouldBeByRef>
		struct shouldPushValueByReference
		{
			enum {value = 0};
			static void push(lua_State* vm, T* input)
			{
				OOLUA::push(vm, *input);
			}
		};

		template<typename T>
		struct shouldPushValueByReference<T, 1>
		{
			enum {value = 1};
			static void push(lua_State* vm, T* input)
			{
				OOLUA::push(vm, input);
			}
		};

		struct PushPublicMember
		{
			template<typename T>
			static void push(lua_State* vm, T* input)
			{
				shouldPushValueByReference<T,
					//can not have a pointer to reference.
					//T will either be a pointer to type
					//or type
					OOLUA::INTERNAL::is_param_by_value<T>::value
					&& has_a_proxy_type<typename OOLUA::INTERNAL::Raw_type<T>::type >::value >
						::push(vm, input);
			}
		};

	} // namespace INTERNAL // NOLINT
	/** \endcond */
} // namespace OOLUA


//TODO Hmm these could use an Optional parameterfor the get/set name
//consider changing this

/** \addtogroup OOLuaDSL
 @{*/

	/** \addtogroup OOLuaExpressive Expressive
	@{*/

	/** \def OOLUA_MEM_GETN
		\hideinitializer
		\brief Generates a getter for a public member, allowing a new to be set for
		the proxy function.
		\details OOLUA_MEM_GETN(get_name,id)
		\param get_name	Name of the proxy function which is generated
		\param id Public member name
	*/
#	define OOLUA_MEM_GETN(get_name, id) \
	int get_name(lua_State* vm) const \
	{ \
		OOLUA::INTERNAL::PushPublicMember::push(vm, &m_this->id); \
		return 1; \
	}

	/** \def OOLUA_MEM_SETN
		\hideinitializer
		\brief Generates a setter for a public member
		\details OOLUA_MEM_SETN(set_name,id)
		\param set_name	Name of the proxy function which is generated
		\param id Public member name
	 */
#	define OOLUA_MEM_SETN(set_name, id) \
	int set_name(lua_State* vm) \
	{ \
		OOLUA::INTERNAL::LUA_CALLED::get(vm, 2, m_this->id); \
		return 0; \
	}


	/**	@}*/


	/** \addtogroup OOLuaMinimalist Minimalist
	@{*/
		/**@{*/
	/** \def OOLUA_MGET
		\hideinitializer
		\details OOLUA_MGET(id)
		\brief Generates a getter for a public member
		\param id Public member name
	*/
#	define OOLUA_MGET(id) OOLUA_MEM_GETN(get_##id, id)

	/** \def OOLUA_MSET
		\hideinitializer
		\details OOLUA_MGET(id)
		\brief Generates a setter for a public member
		\param id Public member name
	 */
#	define OOLUA_MSET(id) OOLUA_MEM_SETN(set_##id, id)

	//TODO this could also use optional parameters consider changing
	/** \def OOLUA_MGET_MSET
		\hideinitializer
		\brief Generates both a getter and setter for a public member
		\details OOLUA_MGET_MSET(id)
		\param id Public member name
	 */
#	define OOLUA_MGET_MSET(id) \
		OOLUA_MGET(id) \
		OOLUA_MSET(id)

		/**@}*/

	/**	@}*/

/**	@}*/
#endif //CLASS_PUBLIC_MEMBER_H_
