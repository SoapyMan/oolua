
#	include "oolua_tests_pch.h"
#	include "oolua.h"
#	include "common_cppunit_headers.h"
#	include "expose_stub_classes.h"
#	include OOLUA_SHARED_HEADER

class SharedPointer : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SharedPointer);
		CPPUNIT_TEST(push_topOfStackIsUserData);
		CPPUNIT_TEST(push_udHasSharedFlagSet);
		CPPUNIT_TEST(push_sharedUseCountEqualsTwo);
		CPPUNIT_TEST(push_pushTheSharedPtrTwice_sharedUseCountEqualsTwo);
		CPPUNIT_TEST(push_pushTheSharedPtrTwice_userdatasCompareEqual);
		CPPUNIT_TEST(gc_pushTheSharedPtrPopItAndThenCallGc_userCountEqualsOne);
#if OOLUA_STORE_LAST_ERROR == 1
		CPPUNIT_TEST(pull_returnEqualsTrue);
#endif
		CPPUNIT_TEST(gc_pushSharedThenPullAndGc_useCountEqualsTwo);
		CPPUNIT_TEST(pull_pushSharedThenPull_resultEqualsInput);
	CPPUNIT_TEST_SUITE_END();
	OOLUA::Script* m_lua;
public:
	SharedPointer()
		: m_lua(0)
	{}
	void setUp()
	{
		m_lua = new OOLUA::Script;
	}
	void tearDown()
	{
		delete m_lua;
	}
	void set_gc_to_false(int index)
	{
		OOLUA::INTERNAL::Lua_ud* ud = static_cast<OOLUA::INTERNAL::Lua_ud*>(lua_touserdata(m_lua->state(), index));
		if (ud) OOLUA::INTERNAL::userdata_gc_value(ud, false);
	}
	void push_topOfStackIsUserData()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		set_gc_to_false(-1);
		CPPUNIT_ASSERT_EQUAL(LUA_TUSERDATA, lua_type(m_lua->state(), -1));
	}
	void push_udHasSharedFlagSet()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		set_gc_to_false(-1);
		OOLUA::INTERNAL::Lua_ud* ud = static_cast<OOLUA::INTERNAL::Lua_ud*>(lua_touserdata(m_lua->state(), -1));
		CPPUNIT_ASSERT_EQUAL(true, OOLUA::INTERNAL::userdata_is_shared_ptr(ud));
	}
	void push_sharedUseCountEqualsTwo()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		set_gc_to_false(-1);
		CPPUNIT_ASSERT_EQUAL(long(2), sp.use_count());
	}
	void push_pushTheSharedPtrTwice_sharedUseCountEqualsTwo()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		m_lua->push(sp);
		set_gc_to_false(-1);
		set_gc_to_false(-2);//we will get a horrible error until the same userdata is used
		CPPUNIT_ASSERT_EQUAL(long(2), sp.use_count());
	}
	void push_pushTheSharedPtrTwice_userdatasCompareEqual()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		m_lua->push(sp);
		set_gc_to_false(-1);
		set_gc_to_false(-2);//we will get a horrible error until the same userdata is used
		CPPUNIT_ASSERT_EQUAL(true, OOLUA::idxs_equal(m_lua->state(), -1, -2));
	}
	void gc_pushTheSharedPtrPopItAndThenCallGc_userCountEqualsOne()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		lua_pop(m_lua->state(), 1);
		m_lua->gc();
		CPPUNIT_ASSERT_EQUAL(long(1), sp.use_count());
	}

#if OOLUA_STORE_LAST_ERROR == 1
	void pull_returnEqualsTrue()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		OOLUA_SHARED_TYPE<Stub1> result;
		CPPUNIT_ASSERT_EQUAL(true, m_lua->pull(result));
	}
#endif
	void gc_pushSharedThenPullAndGc_useCountEqualsTwo()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
		m_lua->push(sp);
		OOLUA_SHARED_TYPE<Stub1> result;
		m_lua->pull(result);
		m_lua->gc();
		CPPUNIT_ASSERT_EQUAL(long(2), result.use_count());
	}
	void pull_pushSharedThenPull_resultEqualsInput()
	{
		m_lua->register_class<Stub1>();
		OOLUA_SHARED_TYPE<Stub1> input(new Stub1);
		m_lua->push(input);
		OOLUA_SHARED_TYPE<Stub1> result;
		m_lua->pull(result);
		CPPUNIT_ASSERT(input == result);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(SharedPointer);

