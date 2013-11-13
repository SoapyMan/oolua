#	include "oolua_tests_pch.h"
#	include "oolua.h"
#	include "common_cppunit_headers.h"
#	include "expose_stub_classes.h"
#	include "expose_class_constructors.h"
#	include "expose_class_ops.h"

struct DefaultConstructorNoShared
{};
OOLUA_PROXY(DefaultConstructorNoShared)
	OOLUA_TAGS(No_shared)
OOLUA_PROXY_END

OOLUA_EXPORT_NO_FUNCTIONS(DefaultConstructorNoShared)

struct ParamConstructorNoShared
{
	ParamConstructorNoShared(int)
	{}
};
OOLUA_PROXY(ParamConstructorNoShared)
	OOLUA_TAGS(
		No_shared
		,No_default_constructor
	)
	OOLUA_CTORS(
		OOLUA_CTOR(int)
	)
OOLUA_PROXY_END

OOLUA_EXPORT_NO_FUNCTIONS(ParamConstructorNoShared)

class SharedByDefault : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SharedByDefault);
		CPPUNIT_TEST(defaultConstructor_createsSharedPtr_topOfStackSharedFlagIsSet);
		CPPUNIT_TEST(paramConstructor_createsSharedPtr_topOfStackSharedFlagIsSet);
		CPPUNIT_TEST(addOperator_createsSharedPtr_topOfStackSharedFlagIsSet);
		CPPUNIT_TEST(subOperator_createsSharedPtr_topOfStackSharedFlagIsSet);
		CPPUNIT_TEST(mulOperator_createsSharedPtr_topOfStackSharedFlagIsSet);
		CPPUNIT_TEST(divOperator_createsSharedPtr_topOfStackSharedFlagIsSet);

		CPPUNIT_TEST(defaultConstructor_hasNoSharedFlag_topOfStackSharedFlagIsNotSet);
		CPPUNIT_TEST(paramConstructor_hasNoSharedFlag_topOfStackSharedFlagIsNotSet);
	CPPUNIT_TEST_SUITE_END();
	OOLUA::Script* m_lua;
public:
	SharedByDefault()
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
	bool stack_index_ud_shared_flag(int index)
	{
		OOLUA::INTERNAL::Lua_ud* ud = static_cast<OOLUA::INTERNAL::Lua_ud*>(lua_touserdata(m_lua->state(), index));
		return OOLUA::INTERNAL::userdata_is_shared_ptr(ud);
	}

	void defaultConstructor_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<Stub1>();
		m_lua->run_chunk("return Stub1.new()");
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void paramConstructor_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<ParamConstructor>();
		m_lua->run_chunk("return ParamConstructor.new(true)");
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void addOperator_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<Class_ops>();

		Class_ops a(1),b(2);
		m_lua->run_chunk("return function(lhs, rhs) return lhs + rhs end");
		m_lua->call(1, &a, &b);
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void subOperator_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<Class_ops>();

		Class_ops a(1),b(2);
		m_lua->run_chunk("return function(lhs, rhs) return lhs - rhs end");
		m_lua->call(1, &a, &b);
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void mulOperator_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<Class_ops>();

		Class_ops a(1),b(2);
		m_lua->run_chunk("return function(lhs, rhs) return lhs * rhs end");
		m_lua->call(1, &a, &b);
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void divOperator_createsSharedPtr_topOfStackSharedFlagIsSet()
	{
		m_lua->register_class<Class_ops>();

		Class_ops a(1),b(2);
		m_lua->run_chunk("return function(lhs, rhs) return lhs / rhs end");
		m_lua->call(1, &a, &b);
		CPPUNIT_ASSERT_EQUAL(true, stack_index_ud_shared_flag(-1));
	}

	void defaultConstructor_hasNoSharedFlag_topOfStackSharedFlagIsNotSet()
	{
		m_lua->register_class<DefaultConstructorNoShared>();
		m_lua->run_chunk("return DefaultConstructorNoShared.new()");
		CPPUNIT_ASSERT_EQUAL(false, stack_index_ud_shared_flag(-1));
	}

	void paramConstructor_hasNoSharedFlag_topOfStackSharedFlagIsNotSet()
	{
		m_lua->register_class<ParamConstructorNoShared>();
		m_lua->run_chunk("return ParamConstructorNoShared.new(1)");
		CPPUNIT_ASSERT_EQUAL(false, stack_index_ud_shared_flag(-1));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SharedByDefault);
