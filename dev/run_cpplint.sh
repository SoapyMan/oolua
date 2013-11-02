#!/bin/bash
cd ..

if [ ! -d build_logs ]; then
	mkdir build_logs
fi

./dev/cpplint.py --filter=\
-whitespace/tab\
,-whitespace/braces\
,-whitespace/comments\
,-whitespace/line_length\
,-whitespace/labels\
,-whitespace/newline\
,-build/header_guard\
,-build/include\
,-runtime/references\
,-runtime/explicit\
,-runtime/int\
,-runtime/sizeof\
,-legal/copyright\
 --root=./include ./include/* ./src/* \
./unit_tests/cpp_classes/* ./unit_tests/test_classes/* ./unit_tests/bind_classes/* \
./unit_tests/string_is_integral/* ./unit_tests/tests_may_fail/* \
./unit_tests/tests_must_fail/cpp_acquire/* \
./unit_tests/tests_must_fail/cpp_in/* \
./unit_tests/tests_must_fail/in_out/* \
./unit_tests/tests_must_fail/lua_acquire/* \
./unit_tests/tests_must_fail/lua_maybe_null/* \
./unit_tests/tests_must_fail/lua_out/* \
./unit_tests/tests_must_fail/lua_return/* \
./unit_tests/tests_must_fail/maybe_null/* \
./unit_tests/tests_must_fail/out/* \
 2> ./build_logs/cpplint.out

# ./unit_tests/cpp_classes/* ./unit_tests/test_classes/* ./unit_tests/bind_classes/* 2> ./build_logs/cpplint.out
# --root=./include ./include/* ./src/* ./unit_tests/* 2> ./build_logs/cpplint.out
#,-legal/copyright

grep -v ^Ignoring ./build_logs/cpplint.out > ./build_logs/cpplint.tmpout
grep -v ^Done ./build_logs/cpplint.tmpout > ./build_logs/cpplint.out
rm ./build_logs/cpplint.tmpout
cd dev
