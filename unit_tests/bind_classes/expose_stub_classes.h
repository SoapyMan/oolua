#ifndef EXPOSE_STUB_CLASSES_H_
#	define EXPOSE_STUB_CLASSES_H_

#include "oolua.h"
#include "cpp_stub_classes.h"

#ifndef OOLUA_WORK_DSL

OOLUA_CLASS_NO_BASES(Stub1)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
OOLUA_CLASS_END


OOLUA_CLASS_NO_BASES(Stub2)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Stub3)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
OOLUA_CLASS_END


OOLUA_CLASS_NO_BASES(InvalidStub)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
OOLUA_CLASS_END



OOLUA_CLASS_NO_BASES(Return_double)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
OOLUA_CLASS_END

#else


OOLUA_PROXY(Stub1)
	OOLUA_TAGS()
	OOLUA_CTORS()
OOLUA_PROXY_END


OOLUA_PROXY(Stub2)
	OOLUA_TAGS()
	OOLUA_CTORS()
OOLUA_PROXY_END

OOLUA_PROXY(Stub3)
	OOLUA_TAGS()
	OOLUA_CTORS()
OOLUA_PROXY_END


OOLUA_PROXY(InvalidStub)
	OOLUA_TAGS()
	OOLUA_CTORS()
OOLUA_PROXY_END


OOLUA_PROXY(Return_double)
	OOLUA_TAGS()
	OOLUA_CTORS()
OOLUA_PROXY_END

#endif


#endif