

#	include "oolua_tests_pch.h"
#	include "oolua.h"
#	include "common_cppunit_headers.h"
#	include "expose_stub_classes.h"
#	include "expose_hierarchy.h"

#	if OOLUA_USE_SHARED_PTR == 1
#		include OOLUA_SHARED_HEADER
int called = 0;
struct SharedFoo
{
	SharedFoo()
		: count(0)
	{}
	void no_param_function_const() const
	{
		++called;
	}
	void no_param_function()
	{
		++count;
	}

	int count;
};

OOLUA_PROXY(SharedFoo)
	OOLUA_MFUNC(no_param_function)
	OOLUA_MFUNC_CONST(no_param_function_const)
OOLUA_PROXY_END

OOLUA_EXPORT_FUNCTIONS(SharedFoo,no_param_function)
OOLUA_EXPORT_FUNCTIONS_CONST(SharedFoo, no_param_function_const)

	class SharedPointer : public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE(SharedPointer);
			CPPUNIT_TEST(push_topOfStackIsUserData);
			CPPUNIT_TEST(push_udHasSharedFlagSet);
			CPPUNIT_TEST(push_sharedUseCountEqualsTwo);
			CPPUNIT_TEST(push_pushTheSharedPtrTwice_sharedUseCountEqualsTwo);
			CPPUNIT_TEST(push_pushTheSharedPtrTwice_userdatasCompareEqual);
			CPPUNIT_TEST(gc_pushTheSharedPtrPopItAndThenCallGc_userCountEqualsOne);
#		if OOLUA_STORE_LAST_ERROR == 1
			CPPUNIT_TEST(pull_returnEqualsTrue);
			CPPUNIT_TEST(pull_pushDerivedThenPullBase_pullReturnsTrue);
#		endif
			CPPUNIT_TEST(gc_pushSharedThenPullAndGc_useCountEqualsTwo);
			CPPUNIT_TEST(pull_pushSharedThenPull_resultEqualsInput);

			CPPUNIT_TEST(push_pushBaseAndThenPushDerived_topTwoIndiciesCompareEqual);
			CPPUNIT_TEST(pull_pushDerivedPullBaseThenCallGc_useCountEqualsTwo);
			CPPUNIT_TEST(push_pushBaseAndThenPushDerived_useCountEqualsTwo);

			CPPUNIT_TEST(call_pushSharedObjectAndCallFunction_callReturnsTrue);
			CPPUNIT_TEST(call_pushSharedObjectAndCallFunction_countMemberEqualsOne);
			CPPUNIT_TEST(call_pushSharedObjectAndCallConstantFunction_globalCalledCountEqualsOne);

			CPPUNIT_TEST(pull_pushSharedPointerPullNormalPointer_pointerEqualsInputsGet);
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

#	if OOLUA_STORE_LAST_ERROR == 1
		void pull_returnEqualsTrue()
		{
			m_lua->register_class<Stub1>();
			OOLUA_SHARED_TYPE<Stub1> sp(new Stub1);
			m_lua->push(sp);
			OOLUA_SHARED_TYPE<Stub1> result;
			CPPUNIT_ASSERT_EQUAL(true, m_lua->pull(result));
		}
#	endif
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

		void push_pushBaseAndThenPushDerived_topTwoIndiciesCompareEqual()
		{
			m_lua->register_class<Derived1Abstract1>();
			OOLUA_SHARED_TYPE<Derived1Abstract1> derived(new Derived1Abstract1);
			OOLUA_SHARED_TYPE<Abstract1> base(derived);

			m_lua->push(base);
			m_lua->push(derived);
			CPPUNIT_ASSERT_EQUAL(true, OOLUA::idxs_equal(m_lua->state(), -1, -2));
		}
#	if OOLUA_STORE_LAST_ERROR == 1
		void pull_pushDerivedThenPullBase_pullReturnsTrue()
		{
			m_lua->register_class<Derived1Abstract1>();
			OOLUA_SHARED_TYPE<Derived1Abstract1> derived(new Derived1Abstract1);
			OOLUA_SHARED_TYPE<Abstract1> base;

			m_lua->push(derived);
			CPPUNIT_ASSERT_EQUAL(true, m_lua->pull(base));
		}
#	endif

		void pull_pushDerivedPullBaseThenCallGc_useCountEqualsTwo()
		{
			m_lua->register_class<Derived1Abstract1>();
			OOLUA_SHARED_TYPE<Derived1Abstract1> derived(new Derived1Abstract1);
			OOLUA_SHARED_TYPE<Abstract1> base;

			m_lua->push(derived);
			m_lua->pull(base);
			m_lua->gc();
			CPPUNIT_ASSERT_EQUAL(long(2), derived.use_count());
		}

		void push_pushBaseAndThenPushDerived_useCountEqualsTwo()
		{
			m_lua->register_class<Derived1Abstract1>();
			OOLUA_SHARED_TYPE<Derived1Abstract1> derived(new Derived1Abstract1);

			{
				//forece a release for base making the counter two
				//one C++ side and one for the two userdatas Lua side
				OOLUA_SHARED_TYPE<Abstract1> base(derived);
				m_lua->push(base);
				m_lua->push(derived);
			}
			CPPUNIT_ASSERT_EQUAL(long(2), derived.use_count());
		}

		void call_pushSharedObjectAndCallFunction_callReturnsTrue()
		{
			m_lua->register_class<SharedFoo>();
			OOLUA_SHARED_TYPE<SharedFoo> foo(new SharedFoo);
			m_lua->run_chunk("return function(obj) obj:no_param_function() end");
			bool result = m_lua->call(1, foo);
			std::cout <<OOLUA::get_last_error(m_lua->state());
			CPPUNIT_ASSERT_EQUAL(true, result);
		}

		void call_pushSharedObjectAndCallFunction_countMemberEqualsOne()
		{
			m_lua->register_class<SharedFoo>();
			OOLUA_SHARED_TYPE<SharedFoo> foo(new SharedFoo);
			m_lua->run_chunk("return function(obj) obj:no_param_function() end");
			m_lua->call(1, foo);
			CPPUNIT_ASSERT_EQUAL(int(1), foo->count);
		}
		void call_pushSharedObjectAndCallConstantFunction_globalCalledCountEqualsOne()
		{
			m_lua->register_class<SharedFoo>();
			OOLUA_SHARED_TYPE<SharedFoo> foo(new SharedFoo);
			m_lua->run_chunk("return function(obj) obj:no_param_function_const() end");
			called = 0;
			m_lua->call(1, foo);
			CPPUNIT_ASSERT_EQUAL(int(1), called);
		}

		//TODO Hmm should this be disallowed?
		void pull_pushSharedPointerPullNormalPointer_pointerEqualsInputsGet()
		{
			m_lua->register_class<SharedFoo>();
			OOLUA_SHARED_TYPE<SharedFoo> input(new SharedFoo);
			SharedFoo* result;
			m_lua->push(input);
			m_lua->pull(result);
			CPPUNIT_ASSERT_EQUAL(input.get(), result);
		}
	};

	CPPUNIT_TEST_SUITE_REGISTRATION(SharedPointer);

#endif //OOLUA_USE_SHARED_POINTER

