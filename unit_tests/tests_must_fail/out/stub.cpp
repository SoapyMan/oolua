#include "oolua.h"
#include "expose_stub_classes.h"

int main()
{
	return OOLUA::out_p<Stub1>::out;
}
