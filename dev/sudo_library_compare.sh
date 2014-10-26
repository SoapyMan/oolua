#!/bin/bash
if [ $EUID != 0 ]; then
    echo  "WARNING:
  	This file is not for general purpose usage and is required to have elevated privileges.
	It will create symlinks for header directories and library names so that it can run
	tests for multiple versions of Lua without modification to the generation files, tests
	and the OOLua library. If you do run this file with the privileges it requires it may
	well remove the Lua library and headers from your system."
	exit 2
fi

if [ "$1" != 'gnu' -a  "$1" != 'xcode' ]; then
	echo "Unknown build requested \""$1"\". Only gnu and xcode supported"
	exit 2
fi
build_system=$1

#Creates symlinks for the Lua version header directory and static library
#$1 Header directory for the Lua version
#$2 Static library name for the Lua version
function setup_headers_and_library()
{
	echo "Setting up header path ${1} and library ${2}"
	sudo rm /usr/local/include/lua
	sudo ln -s /usr/local/include/${1} /usr/local/include/lua
	sudo rm /usr/local/lib/liblua.a
	sudo ln -s /usr/local/lib/${2} /usr/local/lib/liblua.a
}

#$1 Options supplied to Premake when generating the project file
function compile_profile_binary()
{
	cd ..
	premake4 clean
	if [ $build_system == "xcode" ]; then
		premake4 ${1} xcode3 macosx 1 > /dev/null
		cd ./profile
		xcodebuild  -project comparison.xcodeproj -configuration Release >> shhh.txt
		rm shhh.txt
	elif [ $build_system == "gnu" ]; then
		premake4 ${1} gmake linux 1 > /dev/null
		make comparison config=release >> shhh.txt
		rm shhh.txt
		cd ./profile #other parts of the script assume this is the PWD
	fi
}

build_log_dir="../build_logs"

if [ ! -d ${build_log_dir} ]; then
	mkdir ${build_log_dir}
fi
test_ofile="$build_log_dir/profile.txt"

function run_comparison()
{
	../bin/Release/comparison >>${test_ofile}
}

#$1 Doxygen section name
#$2 Lua version
#$3 Type of comparison (either 'Userdata' or 'No userdata' checks)
function prep_table()
{
	echo "Running ${2} ${3} comparisons"
	echo "\\section ${1} ${2} : ${3}" >> ${test_ofile}
	echo "| Library test | cached call | self call |" >> ${test_ofile}
	echo "| :----------- | :---------: | :-------: |" >> ${test_ofile}
}

#Writes the page information for Doxygen, the date of the tests and the CPU for the
#machine on which the tests were ran
function test_info()
{
	echo "/** \\page OOLuaLibraryComparisonResults Comparison results " > ${test_ofile}
	echo $(date) >> ${test_ofile}

	if [ $build_system == "xcode" ]; then
		sysctl -n machdep.cpu.brand_string >> ${test_ofile}
	elif [ $build_system == "gnu" ]; then
		cat /proc/cpuinfo | grep "model name"|head -n 1|  cut -d ":" -f2 | tr -s " " >> ${test_ofile}
	fi
}

#$1 Name of the binary which will be ran to query the point version
#$2 The variable to write the resulting point version to
function lua_point_version()
{
	local _result=$2
	local _local_result="$(($1 -v) 2>&1 | cut -d 'C' -f 1 | tr -d '-')"
	eval $_result="'$_local_result'"
}

function run_lua51()
{
	#setup Lua 5.1
	lua_version="5.1"
	local point_version=1
	lua_point_version "lua-5.1" point_version

	setup_headers_and_library lua51 liblua-${lua_version}.a

#	compile_profile_binary  "--LUABIND_COMPARE --LUABRIDGE_COMPARE"
	compile_profile_binary  "--LUABRIDGE_COMPARE"

	prep_table "lua51Checks" "${point_version}" "Userdata checks"
	run_comparison

	export NO_USERDATA_CHECKS=1
#	compile_profile_binary "--LUABIND_COMPARE --SWIG_COMPARE"
	compile_profile_binary "--SWIG_COMPARE"

	prep_table "lua51NoChecks" "${point_version}" "No userdata checks"
	run_comparison
	unset NO_USERDATA_CHECKS
}

function run_lua52()
{
	#setup Lua 5.2
	lua_version="5.2"
	local point_version=1
	lua_point_version "lua-5.2" point_version

	setup_headers_and_library lua52 liblua-${lua_version}.a

#	compile_profile_binary "--LUABIND_COMPARE --LUABRIDGE_COMPARE"
	compile_profile_binary "--LUABRIDGE_COMPARE"

	prep_table "lua52Checks" "${point_version}" "Userdata checks"

	run_comparison

	export NO_USERDATA_CHECKS=1
#	compile_profile_binary "--LUABIND_COMPARE --SWIG_COMPARE --SLB3_COMPARE"
	compile_profile_binary "--SWIG_COMPARE --SLB3_COMPARE"

	prep_table "lua52NoChecks" "${point_version}" "No userdata checks"

	run_comparison
	unset NO_USERDATA_CHECKS
}

function run_luajit1()
{
	#setup LuaJIT 1
	lua_version="5.1.1.1.8"
	setup_headers_and_library lua51 libluajit-${lua_version}.a

	export LUAJIT_1=1
	compile_profile_binary "--LUABIND_COMPARE --LUABRIDGE_COMPARE"
	prep_table "luaJIT1Checks" "LuaJIT ${lua_version}" "Userdata checks"
	run_comparison

	export NO_USERDATA_CHECKS=1
	compile_profile_binary "--LUABIND_COMPARE --SWIG_COMPARE"
	prep_table "luaJIT1NoChecks" "LuaJIT ${lua_version}" "No userdata checks"
	run_comparison
	unset NO_USERDATA_CHECKS
	unset LUAJIT_1
}

function run_luajit2()
{
	#setup LuaJIT 2

	local point_version=1
	lua_point_version "luajit" point_version

	setup_headers_and_library luajit-2.0 libluajit-5.1.2.a

#	setup_headers_and_library luajit-2.0 libluajit-${lua_version}.a

#	compile_profile_binary "--JIT_REBASE --LUABIND_COMPARE --LUABRIDGE_COMPARE"
	compile_profile_binary "--JIT_REBASE --LUABRIDGE_COMPARE"
	prep_table "luaJIT2Checks" "${point_version}" "Userdata checks"

	run_comparison

	export NO_USERDATA_CHECKS=1
#	compile_profile_binary "--JIT_REBASE --LUABIND_COMPARE --SWIG_COMPARE"
	compile_profile_binary "--JIT_REBASE --SWIG_COMPARE"
	prep_table "luaJIT2NoChecks" "${point_version}" "No userdata checks"

	run_comparison
	unset NO_USERDATA_CHECKS
}

test_info

run_lua51
run_lua52

#run_luajit1
run_luajit2

echo "*/" >> ${test_ofile}

#now reset Lua 5.2 as the default for this machine
setup_headers_and_library lua52 liblua-5.2.a

#remove all traces whilst we are still sudo
sudo chmod -R 775 ${build_log_dir}
cd ..
premake4 clean


