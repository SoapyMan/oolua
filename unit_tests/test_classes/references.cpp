#	include "oolua_tests_pch.h"
#	include "oolua.h"
#	include "common_cppunit_headers.h"
#	include "expose_stub_classes.h"

class References : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(References);
		CPPUNIT_TEST(funcRefs_refsToSameFunction_equalityReturnsTrue);
		CPPUNIT_TEST(funcRefs_differentRefs_equalityReturnsFalse);
		CPPUNIT_TEST(funcRefs_equalityCleansTheStack_stackSizeIsZero);
		CPPUNIT_TEST(funcRefs_twoDifferentFromRelatedStates_equalityReturnsFalse);
		CPPUNIT_TEST(funcRefs_sameFunctionInDifferentYetRelatedStates_equalityReturnsTrue);
		CPPUNIT_TEST(funcRefs_validAndInvalidRefNoState_equalityReturnsFalse);
		CPPUNIT_TEST(funcRefs_InvalidNoStateAndValidRef_equalityReturnsFalse);
		CPPUNIT_TEST(funcRefs_InvalidHasSameStateAsValidRef_equalityReturnsFalse);
		CPPUNIT_TEST(funcRefs_twoInvalidRefsWithNoState_equalityReturnsTrue);
		CPPUNIT_TEST(funcRefs_InvalidWithStateAndValidWithSameState_equalityReturnsFalse);
		CPPUNIT_TEST(ref_twoRefsOfDifferentTypes_returnsFalse);
	CPPUNIT_TEST_SUITE_END();
	OOLUA::Script * m_lua;
public:
	References()
		: m_lua(0)
	{}
	References(References const&);
	References& operator =(References const&);
	void setUp()
	{
		m_lua = new OOLUA::Script;
	}
	void tearDown()
	{
		delete m_lua;
	}
	void funcRefs_refsToSameFunction_equalityReturnsTrue()
	{
		m_lua->load_chunk("return ");
		lua_pushvalue(*m_lua, 1);
		OOLUA::Lua_func_ref one, two;
		m_lua->pull(one);
		m_lua->pull(two);
		CPPUNIT_ASSERT_EQUAL(true, one == two);
	}

	void funcRefs_differentRefs_equalityReturnsFalse()
	{
		m_lua->load_chunk("return ");
		m_lua->load_chunk("return ");
		OOLUA::Lua_func_ref one, two;
		m_lua->pull(one);
		m_lua->pull(two);
		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}

	void funcRefs_equalityCleansTheStack_stackSizeIsZero()
	{
		m_lua->load_chunk("return ");
		lua_pushvalue(*m_lua, 1);
		OOLUA::Lua_func_ref one, two;
		m_lua->pull(one);
		m_lua->pull(two);
		bool dummy_result(one == two);
		(void)dummy_result;
		CPPUNIT_ASSERT_EQUAL(0, lua_gettop(*m_lua));
	}

	void funcRefs_twoDifferentFromRelatedStates_equalityReturnsFalse()
	{
		OOLUA::Lua_func_ref one, two;
		m_lua->load_chunk("return ");
		m_lua->pull(one);

		lua_State* vm = lua_newthread(*m_lua);
		OOLUA::load_chunk(vm, "return ");
		OOLUA::pull(vm, two);

		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}

	void funcRefs_sameFunctionInDifferentYetRelatedStates_equalityReturnsTrue()
	{
		OOLUA::Lua_func_ref one, two;
		m_lua->load_chunk("return ");
		m_lua->pull(one);

		lua_State* vm = lua_newthread(*m_lua);
		m_lua->push(one);
		lua_xmove(*m_lua, vm, 1);
		OOLUA::pull(vm, two);

		CPPUNIT_ASSERT_EQUAL(true, one == two);
	}

	void funcRefs_validAndInvalidRefNoState_equalityReturnsFalse()
	{
		OOLUA::Lua_func_ref one, two;
		m_lua->load_chunk("return ");
		m_lua->pull(one);
		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}
	void funcRefs_InvalidNoStateAndValidRef_equalityReturnsFalse()
	{
		OOLUA::Lua_func_ref one, two;
		m_lua->load_chunk("return ");
		m_lua->pull(two);
		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}

	void funcRefs_InvalidHasSameStateAsValidRef_equalityReturnsFalse()
	{
		OOLUA::Lua_func_ref one, two;
		m_lua->load_chunk("return ");
		m_lua->pull(two);
		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}
	void funcRefs_twoInvalidRefsWithNoState_equalityReturnsTrue()
	{
		OOLUA::Lua_func_ref one, two;
		CPPUNIT_ASSERT_EQUAL(true, one == two);
	}
	void funcRefs_InvalidWithStateAndValidWithSameState_equalityReturnsFalse()
	{
		OOLUA::Lua_func_ref one(*m_lua), two;
		m_lua->load_chunk("return ");
		m_lua->pull(two);
		CPPUNIT_ASSERT_EQUAL(false, one == two);
	}
	void ref_twoRefsOfDifferentTypes_returnsFalse()
	{
		OOLUA::Lua_func_ref f;
		OOLUA::Lua_table_ref t;
		CPPUNIT_ASSERT_EQUAL(false, t == f);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(References);
