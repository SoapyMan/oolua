#!/bin/bash

if [ "$1" != 'gnu' -a  "$1" != 'xcode' ]; then
	echo "Unknown build requested \""$1"\". Only gnu and xcode supported"
	exit 2
fi
test_script=$1


if [ $EUID != 0 ]; then
	echo  "WARNING:
	This file is not for general purpose usage and is required to have
	elevated privileges. It will create symlinks for header directories
	and library names so that it can run tests for multiple versions of
	Lua without modification to the generation files, tests and the
	OOLua library.
	If you do run this file with the privileges it requires it may well
	remove the Lua library and headers from your system."
	exit 2
fi

#1: header directory 2: library name
function setup_headers_and_library()
{
	sudo rm /usr/local/include/lua
	sudo ln -s /usr/local/include/${1} /usr/local/include/lua
	sudo rm /usr/local/lib/liblua.a
	sudo ln -s /usr/local/lib/${2} /usr/local/lib/liblua.a
}

#$1 Name of the binary which will be ran to query the point version
#$2 The variable to write the resulting point version to
function lua_point_version()
{
	local _result=$2
	local _local_result="$(($1 -v) 2>&1 | cut -d 'C' -f 1 | tr -d '-')"
	eval $_result="'$_local_result'"
}

time_start=$(date +%s)
cd ../build_scripts

setup_headers_and_library lua51 liblua-5.1.a
point_version=1
lua_point_version "lua-5.1" point_version
echo "Running ${point_version} tests"
./${test_script}_tests.sh

setup_headers_and_library lua52 liblua-5.2.a
lua_point_version "lua-5.2" point_version
echo "Running ${point_version} tests"
./${test_script}_tests.sh

setup_headers_and_library lua53 liblua-5.3.a
lua_point_version "lua-5.3" point_version
echo "Running ${point_version} tests"
./${test_script}_tests.sh

setup_headers_and_library luajit-2.0 libluajit-5.1.2.a
#OSX x86_64 executables which use LuaJIT require rebasing so that the JIT allocator can use the lowest 2GB
export LUAJIT_REBASE=1
lua_point_version "luajit-2" point_version
echo "Running ${point_version} tests"
./${test_script}_tests.sh
unset LUAJIT_REBASE

<<ThisRequiresCppUnitAndGoogleMockToBeBuiltForx86
setup_headers_and_library lua51 libluajit-5.1.1.1.8.a
echo "Running LuaJIT 1.1.8 tests"
export LUAJIT_32BIT=1
./${test_script}_tests.sh
unset LUAJIT_32BIT
ThisRequiresCppUnitAndGoogleMockToBeBuiltForx86

time_end=$(date +%s)
time_diff=$(( $time_end - $time_start ))
minutes=$(( $time_diff / 60 ))
seconds=$(printf %02d $(( $time_diff % 60 )))
echo "Finished tests for multiple versions of Lua in $minutes:$seconds"

#now reset Lua 5.2 as the default for this machine
setup_headers_and_library lua52 liblua-5.2.a

#remove all traces whilst we are still sudo
sudo chmod -R 775 ../build_logs

cd ../dev

