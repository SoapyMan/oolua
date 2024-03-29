--oolua premake4 file

dofile("helper4.lua")

if _ACTION == "clean" then
  os.rmdir("obj")
  os.rmdir("bin")
  os.rmdir("ipch")
end

solution("oolua")
   configurations { "Debug", "Release" }
   if os.is64bit() and _ACTION and _ACTION:match("^vs") then 
 	platforms{"x86", "x64"}
   end


dofile("./oolua4.lua")
dofile("./unit_tests/test.unit.lua")
include("./unit_tests/tests_may_fail/")
include("./profile/")
include("./unit_tests/string_is_integral/")
dofile("./unit_tests/test.unit.exceptions.lua")
dofile("./unit_tests/test.coverage.lua")
include("./unit_tests/shared_by_default/")
include("./unit_tests/shared/")
include("./unit_tests/scoped_enum/")

include("./unit_tests/tests_must_fail/lua_out/")
include("./unit_tests/tests_must_fail/cpp_in/")
include("./unit_tests/tests_must_fail/out/")
include("./unit_tests/tests_must_fail/in_out/")
include("./unit_tests/tests_must_fail/lua_return/")
include("./unit_tests/tests_must_fail/cpp_acquire/")
include("./unit_tests/tests_must_fail/lua_acquire/")
include("./unit_tests/tests_must_fail/maybe_null/")
include("./unit_tests/tests_must_fail/lua_maybe_null/")
include("./unit_tests/tests_must_fail/shared_return/")

local gen = require'build_scripts.oolua_generate'
local gen_details = gen.default_details()
newoption
{
	trigger = 'cpp_params'
	,value = 'Number'
	,description = 'oolua-gen option\n\t' .. gen_details.cpp_params.desc
}
newoption
{
	trigger = 'lua_params'
	,value = 'Number'
	,description = 'oolua-gen option\n\t' .. gen_details.lua_params.desc
}
newoption
{
	trigger = 'constructor_params'
	,value = 'Number'
	,description = 'oolua-gen option\n\t' .. gen_details.constructor_params.desc
}

gen_defaults=nil
gen=nil

newaction
{
	trigger     = 'oolua-gen',
	description = 'Generate OOLua boilerplate files',
	execute = function ()
		local gen = require'build_scripts.oolua_generate'
		local gen_options =
		{
			cpp_params = _OPTIONS.cpp_params and tonumber(_OPTIONS.cpp_params) or nil
			,lua_params = _OPTIONS.lua_params and tonumber(_OPTIONS.lua_params) or nil
			,constructor_params = _OPTIONS.constructor_params and tonumber(_OPTIONS.constructor_params) or nil
		}
		gen.gen(gen_options,'include/')
   	end
}

newaction
{
	trigger     = 'oolua-amalgam',
	description = 'Amalgamate OOLua',
	execute = function ()
		os.mkdir("./amal")
		require('build_scripts.oolua_amalgamation').amalgamate('./include/', './src/', './amal/')
	end
}
