#ifndef CPP_USERDATA_RETURNS_H_
#	define CPP_USERDATA_RETURNS_H_


#	include "cpp_function_returns.h"
#	include "cpp_stub_classes.h"
struct oolua_test_userdata_returns_default_traits{};
struct oolua_test_userdata_return_traits{};


typedef FunctionReturnType<oolua_test_userdata_returns_default_traits,Stub1> UserDataFunctionDefaultReturnTraits;
typedef MockFunctionReturnType<oolua_test_userdata_returns_default_traits,Stub1> UserDataFunctionDefaultReturnTraitsMock;

typedef FunctionReturnType<oolua_test_userdata_return_traits,Stub1> UserDataFunctionReturnTraits;
typedef MockFunctionReturnType<oolua_test_userdata_return_traits,Stub1> UserDataFunctionReturnTraitsMock;

#endif
