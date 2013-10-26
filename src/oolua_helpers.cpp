#include "lua_includes.h"

namespace OOLUA
{

	bool idxs_equal(lua_State* vm, int idx0, int idx1)
	{
#	if LUA_VERSION_NUM < 502
		return !!lua_equal(vm, idx0, idx1);
#	else
		return !!lua_compare(vm, idx0, idx1, LUA_OPEQ);
#	endif
	}


	bool can_xmove(lua_State* vm0, lua_State* vm1)
	{
		if(!vm0 || !vm1 || vm0 == vm1) return false;

		/*
		Threads that are related share the same registry
		G(vm0)->l_registry == G(vm1)->l_registry
		*/
		return lua_topointer(vm0, LUA_REGISTRYINDEX) == lua_topointer(vm1, LUA_REGISTRYINDEX);
	}

} // namespace OOLUA
