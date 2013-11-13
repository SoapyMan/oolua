//This file is not part of OOLua, the only reason for it is to allow doxygen to document
//some things which otherwise it can not do.
/**
	\file only_for_doxygen.h
	\brief This file is not part of OOLua, the only reason for it is to allow
	doxygen to document some things which otherwise it can not do.
*/
/**
	\brief Lua's C function signature.
	\details This is a Lua type which is the required signature to bind
	C functions to Lua.
	\param[in] vm The virtual machine for which a function will operate on
	\return Number of function returns to Lua
*/
typedef int (*lua_CFunction)(lua_State* vm);

namespace std
{
	/**
	*/
	struct exception{};
}

#define OOLUA_SHARED_IS_DEFAULT_FOR_CTOR_AND_OPERATOR
#define OOLUA_SHARED_HEADER
#define OOLUA_SHARED_TYPE
#define OOLUA_SHARED_CONST_CAST
