#ifndef OOLUA_MEMBER_FUNCTION_H_
#	define OOLUA_MEMBER_FUNCTION_H_

#include "lua_includes.h"
#include "proxy_class.h"
#include "class_from_stack.h"
#include "oolua_config.h"


#if	OOLUA_USE_EXCEPTIONS ==1
#	include "oolua_error.h"

#	define OOLUA_PROXY_CALL_CATCH_RESPONSE(exception_type,what_message)\
		luaL_error(l, "Type of exception: %s.\n what(): %s.\n When calling function on proxy type: %s\n" \
					, exception_type \
					, what_message   \
					, Proxy_type::class_name );



#	define OOLUA_MEMBER_FUNCTION_TRY \
	try \
	{ /*start try block*/ 



#	define OOLUA_MEMBER_FUNCTION_CATCH \
	} /*end try block */ \
	catch (OOLUA::Type_error const & e)\
	{\
		OOLUA_PROXY_CALL_CATCH_RESPONSE("OOLUA::Type_error",e.what())\
	}\
	catch (std::runtime_error const & e) \
	{\
		OOLUA_PROXY_CALL_CATCH_RESPONSE("std::runtime_error",e.what())\
	}\
	catch (...) \
	{\
		OOLUA_PROXY_CALL_CATCH_RESPONSE("Unknown type"," " )\
	}\
	/*can not return here yet required for the function signature */ \
	/* as it would have either returned in the */ \
	/*try block or called luaL_error which never returns */ \
	return 0;

#endif



namespace OOLUA
{
	namespace INTERNAL
	{

		template<typename Proxy_type,typename Base_type>int member_caller(lua_State* /*const*/ );
		template<typename Proxy_type,typename Base_type>int const_member_caller(lua_State* /*const*/ );


		///////////////////////////////////////////////////////////////////////////////
		///  inline public  member_caller
		///  Member function dispatcher
		///  @param [in]        lua_State *const \copydoc lua_State
		///  @return int requirement of Lua functions
		///////////////////////////////////////////////////////////////////////////////
		template<typename Proxy_type, typename Base_type>
		inline int member_caller(lua_State * /*const*/ l)
		{
#if	OOLUA_USE_EXCEPTIONS ==1			
			OOLUA_MEMBER_FUNCTION_TRY
#endif
				typename Proxy_type::class_ *obj = 
						INTERNAL::no_stack_checks_none_const_class_from_index<typename Proxy_type::class_>(l, 1);
				lua_remove(l, 1);
				///get member function from upvalue
				typename Proxy_class<Base_type >::Reg_type* r =
						static_cast<typename Proxy_class<Base_type >::Reg_type*>(lua_touserdata(l, lua_upvalueindex(1)));
				Proxy_type P(obj);
				return (P.*(r->mfunc))(l);  ///call member function
#if	OOLUA_USE_EXCEPTIONS ==1
			OOLUA_MEMBER_FUNCTION_CATCH
#endif
		}
		template<typename Proxy_type, typename Base_type>
		inline int const_member_caller(lua_State * /*const*/ l)
		{
#if	OOLUA_USE_EXCEPTIONS ==1			
			OOLUA_MEMBER_FUNCTION_TRY
#endif
				typename Proxy_type::class_ *obj = 
						INTERNAL::no_stack_checks_class_from_index<typename Proxy_type::class_>(l, 1);
				lua_remove(l, 1);
				///get member function from upvalue
				typename Proxy_class<Base_type >::Reg_type_const* r =
						static_cast<typename Proxy_class<Base_type >::Reg_type_const*>(lua_touserdata(l, lua_upvalueindex(1)));
				Proxy_type P(obj);
				return (P.*(r->mfunc))(l);  ///call member function
#if	OOLUA_USE_EXCEPTIONS ==1	
			OOLUA_MEMBER_FUNCTION_CATCH
#endif
		}
	}

}




#if	OOLUA_USE_EXCEPTIONS == 1
#	undef OOLUA_PROXY_CALL_CATCH_RESPONSE
#	undef OOLUA_MEMBER_FUNCTION_TRY
#	undef OOLUA_MEMBER_FUNCTION_CATCH
#endif

#endif
