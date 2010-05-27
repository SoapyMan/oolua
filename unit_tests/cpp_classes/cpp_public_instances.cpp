#include "cpp_public_instances.h"

namespace
{
	Stub1 bad_namespaced_instance;
}



const int Public_variables::set_value;
const int Public_variables::initail_value;


Public_variables::Public_variables()
	:an_int(initail_value)
	,int_ptr(new int(initail_value))
	,dummy_instance(new Stub1)
	,dummy_ref(bad_namespaced_instance)
{}

Public_variables::~Public_variables()
{
	delete int_ptr;
	delete dummy_instance;
}