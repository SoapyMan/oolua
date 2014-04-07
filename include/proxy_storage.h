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

#ifndef OOLUA_PROXY_STORAGE_H_
#	define OOLUA_PROXY_STORAGE_H_

#include "lua_includes.h"
#include "oolua_traits_fwd.h"
#include "oolua_stack_fwd.h"
#include "oolua_config.h"
#include "proxy_userdata.h"
#include "proxy_base_checker.h"
#include "proxy_class.h"

#include "lvd_types.h"
#include "class_from_stack.h"
#include "type_list.h"
#include "char_arrays.h"


namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		typedef bool (*is_const_func_sig)(Lua_ud const* ud);
		template<int NotTheSameSize>
		struct VoidPointerSameSizeAsFunctionPointer;

		template<int NotTheSameSize>
		struct VoidPointerSameSizeAsFunctionPointer
		{
			static void getWeakTable(lua_State* vm)
			{
				lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::INTERNAL::weak_lookup_name);
			}
			static void setWeakTable(lua_State* vm, int value_index)
			{
				OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::weak_lookup_name);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
			static void getCollisionMetatable(lua_State* vm)
			{
				lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::INTERNAL::collision_mt_lookup_name);
			}
			static void setCollisionMetatable(lua_State* vm, int value_index)
			{
				OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::collision_mt_lookup_name);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
		};


		template<>
		struct VoidPointerSameSizeAsFunctionPointer< sizeof(is_const_func_sig) >
		{
			static void getWeakTable(lua_State* vm)
			{
				//it is safe as the pointers are the same size
				//yet we need to stop warnings
				//NOTE: in 5.2 we can push a light c function here
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_constant;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_rawget(vm, LUA_REGISTRYINDEX);
			}
			static void setWeakTable(lua_State* vm, int value_index)
			{
				//it is safe as the pointers are the same size
				//yet we need to stop warnings
				//NOTE: in 5.2 we can push a light c function here
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_constant;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
			static void getCollisionMetatable(lua_State* vm)
			{
				//it is safe as the pointers are the same size
				//yet we need to stop warnings
				//NOTE: in 5.2 we can push a light c function here
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_to_be_gced;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_rawget(vm, LUA_REGISTRYINDEX);
			}
			static void setCollisionMetatable(lua_State* vm, int value_index)
			{
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_to_be_gced;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
		};

		typedef VoidPointerSameSizeAsFunctionPointer<sizeof(void*)> Weak_table; // NOLINT

		//pushes the weak top and returns its index
		int push_weak_table(lua_State* vm);
		template<typename T>Lua_ud* add_ptr(lua_State* vm, T* const ptr, bool is_const, Owner owner);

		template<typename T>Lua_ud* find_ud(lua_State* vm, T* ptr, bool is_const);

		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr);
		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr, int tableIndex);

		template<typename T>
		Lua_ud* reset_metatable(lua_State*  vm, T* ptr, bool is_const);

		void add_ptr_if_required(lua_State* const vm, void* ptr, int udIndex, int weakIndex);


		Lua_ud* new_userdata(lua_State* vm, void* ptr, bool is_const
							 , oolua_function_check_base base_checker, oolua_type_check_function type_check);
		void reset_userdata(Lua_ud* ud, void* ptr, bool is_const
							, oolua_function_check_base base_checker, oolua_type_check_function type_check);

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Add_ptr;

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Has_a_root_entry;

		template<typename T>
		int lua_set_owner(lua_State* vm);

		bool ud_at_index_is_const(lua_State* vm, int index);


		template<typename T>
		int lua_set_owner(lua_State*  vm)
		{
			T* p = check_index<T>(vm, 1);
			if(!p) return luaL_error(vm, "The self/this instance to '%s' is not of type '%s'"
									 , OOLUA::INTERNAL::set_owner_str
									 , OOLUA::Proxy_class<T>::class_name);

			Owner own(No_change);
			OOLUA::INTERNAL::LUA_CALLED::get(vm, 2, own);
			if(own != No_change) INTERNAL::userdata_gc_value(static_cast<INTERNAL::Lua_ud*>(lua_touserdata(vm, 1)), own == Cpp? false : true);
			return 0;
		}

		template<typename T>
		inline bool is_derived_ptr(oolua_type_check_function base_type_checker, T* ptr)
		{
			Lua_ud fake_stack_ud = {ptr, 0 , register_class_imp<T>, 0};
			Lua_ud fake_requested_ud = {0, 0, base_type_checker, 0};
			requested_ud_is_a_base<T>(&fake_requested_ud, &fake_stack_ud);
			return fake_requested_ud.void_class_ptr != NULL;
		}

		template<typename T>
		inline Lua_ud* is_correct_ud(lua_State* vm, Lua_ud* ud, T* ptr, bool const is_const)
		{
			bool const was_const = OOLUA::INTERNAL::userdata_is_constant(ud);
			if (ud_is_type<T>(ud) || valid_base_ptr_or_null<T>(ud))
			{
				ud->flags &= ((!is_const && was_const) ? (~CONST_FLAG) : ud->flags);
				return ud;
			}
			else if (is_derived_ptr(ud->type_check, ptr))
				return reset_metatable(vm, ptr, was_const && is_const);

			return NULL;
		}

		template<typename T>
		inline Lua_ud* check_roots(lua_State* vm, T * ptr, bool const is_const, int cache_table_index)
		{
			/*
				possibilities:
				a base class is stored.
				none of the hierarchy is stored
			*/
			Lua_ud* ud(0);
			bool base_is_stored(false);
			Has_a_root_entry<
					T
					, typename FindRootBases<T>::Result
					, 0
					, typename TYPELIST::At_default< typename FindRootBases<T>::Result, 0, TYPE::Null_type >::Result
				> checkRoots;
			checkRoots(vm, ptr, cache_table_index, base_is_stored);
			if(base_is_stored)
			{
				bool was_const = ud_at_index_is_const(vm, -1);
				ud = reset_metatable(vm, ptr, was_const && is_const);
			}
			return ud;
		}


		//if found it is left on the top of the stack and returns the Lua_ud ptr
		//else the stack is same as on entrance to the function and null is returned
		template<typename T>
		inline Lua_ud* find_ud(lua_State* vm, T * ptr, bool const is_const)
		{
			Lua_ud* ud(0);
			int cache_table_index = push_weak_table(vm);
			lua_pushlightuserdata(vm, ptr);
			lua_rawget(vm, cache_table_index);
			switch(lua_type(vm, -1))
			{
				case LUA_TNIL: //no cache entry
				{
					lua_pop(vm, 1); //pop the nil
					ud = check_roots(vm, ptr, is_const, cache_table_index);
					lua_remove(vm, cache_table_index);
					break;
				}
				case LUA_TUSERDATA: //one cached entry
				{
					ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));
					if ((ud = is_correct_ud(vm, ud, ptr, is_const)) == NULL)
					{
						lua_pop(vm, 1);//pop the found ud
						ud = check_roots(vm, ptr, is_const, cache_table_index);
						lua_remove(vm, cache_table_index);
					}
					else
						lua_remove(vm, cache_table_index);

					break;
				}
				case LUA_TTABLE: //pointer collision
				{
					lua_pushnil(vm);
					while(lua_next(vm, -2) != 0)
					{
						ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));
						if ((ud = is_correct_ud(vm, ud, ptr, is_const)) == NULL)
							lua_pop(vm, 1);//pop the ud value to continue iteration
						else
						{
							lua_replace(vm, cache_table_index);
							lua_settop(vm, cache_table_index);
							return ud;
						}
					}
					lua_pop(vm, 2); //remove cache_table and collision table
					break;
				}
			}

			return ud;
		}

		template<typename T>
		inline Lua_ud* reset_metatable(lua_State* vm, T* ptr, bool is_const)
		{
			Lua_ud *ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));//ud
			reset_userdata(ud, ptr, is_const, &requested_ud_is_a_base<T>, &register_class_imp<T>);
			//change the metatable associated with the ud
			lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::Proxy_class<T>::class_name);

			lua_setmetatable(vm, -2);//set ud's metatable to this

			int weak_index = push_weak_table(vm);//ud weakTable
			//then register all the bases that need it
			Add_ptr<T
					, typename OOLUA::Proxy_class<T>::AllBases
					, 0
					, typename TYPELIST::At_default< typename OOLUA::Proxy_class<T>::AllBases, 0, TYPE::Null_type >::Result
				> addThisTypesBases;
			addThisTypesBases(vm, ptr, weak_index-1, weak_index);
			lua_pop(vm, 1);//ud
			return ud;
		}

		template<typename T>
		inline Lua_ud* add_ptr(lua_State* const vm, T* const ptr, bool is_const, Owner owner)
		{
			Lua_ud* ud = new_userdata(vm, ptr, is_const, &requested_ud_is_a_base<T>, &register_class_imp<T>);
			if(owner != No_change)userdata_gc_value(ud, owner == Lua);

			lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::Proxy_class<T>::class_name);

#if	OOLUA_DEBUG_CHECKS == 1
			assert(lua_isnoneornil(vm, -1) == 0 && "no metatable of this name found in registry");
#endif
			////Pops a table from the stack and sets it as the new metatable for the value at the given acceptable index
			lua_setmetatable(vm, -2);

			int weakIndex = push_weak_table(vm);//ud,weakTable
			int udIndex = weakIndex -1;

			add_ptr_if_required(vm, ptr, udIndex, weakIndex);//it is required

			Add_ptr<T
					, typename OOLUA::Proxy_class<T>::AllBases
					, 0
					, typename TYPELIST::At_default< typename OOLUA::Proxy_class<T>::AllBases, 0, TYPE::Null_type >::Result
				> addThisTypesBases;
			addThisTypesBases(vm, ptr, udIndex, weakIndex);

			lua_pop(vm, 1);//ud
			return ud;
		}

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Add_ptr
		{
			void operator()(lua_State * const vm, Type* ptr, int udIndex, int weakIndex)
			{
				//add this type if needed
				add_ptr_if_required(vm, static_cast<BaseType*>(ptr), udIndex, weakIndex);
				//add the next in the type list if needed
				Add_ptr<
						Type
						, Bases
						, BaseIndex + 1
						, typename TYPELIST::At_default< Bases, BaseIndex + 1, TYPE::Null_type >::Result
					> addBaseNextPtr;
				addBaseNextPtr(vm, ptr, udIndex, weakIndex);
			}
		};

		template<typename Type, typename Bases, int BaseIndex>
		struct Add_ptr<Type, Bases, BaseIndex, TYPE::Null_type>
		{
			void operator()(lua_State * const /*vm*/, Type* /*ptr*/, int /*udIndex*/, int /*weakIndex*/)const
			{}
		};

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Has_a_root_entry
		{
			void operator()(lua_State * const vm, Type* ptr, int weakIndex, bool& result)
			{
				if(result)return;

				void* void_base_ptr = static_cast<void*>(static_cast<BaseType*>(ptr));
				if(static_cast<void*>(ptr) != void_base_ptr
				   && is_there_an_entry_for_this_void_pointer(vm, void_base_ptr, weakIndex))
				{
					switch(lua_type(vm, -1))
					{
						case LUA_TUSERDATA:
						{
							Lua_ud* ud = static_cast<Lua_ud*>(lua_touserdata(vm, -1));
							if( is_derived_ptr(ud->type_check, ptr))
							{
								result = true;
								return;
							}
							else
								lua_pop(vm, 1);//remove the incorrect ud

							break;
						}
						case LUA_TTABLE:
						{
							lua_pushnil(vm);//collisionTable, nil
							while(lua_next(vm, -2) != 0)
							{
								//collisionTable, ud, ud
								Lua_ud* ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));
								if( is_derived_ptr(ud->type_check, ptr))
								{
									result = true;
									lua_replace(vm, -3);
									lua_pop(vm, 1);
									return;
								}
								//keep the ud(key) for the next iteration
								lua_pop(vm, 1);//collisionTable, ud
							}
							lua_pop(vm, 1);//remove the collision table
							break;
						}
					}
				}
				Has_a_root_entry<
						Type
						, Bases
						, BaseIndex + 1
						, typename TYPELIST::At_default< Bases, BaseIndex + 1, TYPE::Null_type >::Result
					> checkNextBase;
				checkNextBase(vm, ptr, weakIndex, result);
			}
		};

		template<typename Type, typename Bases, int BaseIndex>
		struct Has_a_root_entry<Type, Bases, BaseIndex, TYPE::Null_type>
		{
			void operator()(lua_State * const /*vm*/, Type* /*ptr*/, int /*weakIndex*/, bool& /*result*/)const
			{}
		};


	} // namespace INTERNAL // NOLINT
	/**\endcond*/


} // namespace OOLUA

#endif //OOLUA_PROXY_STORAGE_H_
