#!/bin/bash

function failed()
{
    echo "Failed: $@" >&2
    exit 1
}

#1 : debug or release
#2 : project name
#3 : store_error or using_exceptions
function compile_and_run()
{
	echo "building  ${2} | ${1} | ${3} : "
	make config=${1} ${2} >  ./build_logs/${2}.${1}_${3}.log || failed ${1} build;
	echo "build passed"
}

set -e

cd ..
premake4 clean
premake4 gmake linux > /dev/null

if [ ! -d build_logs ]; then
	mkdir build_logs
fi

rm -rf build_logs/coverage_report

compile_and_run debug test.coverage store_error
compile_and_run debug test.coverage.using_exceptions exceptions

compile_and_run debug test.coverage.string_is_integral store_error
compile_and_run debug test.coverage.string_is_integral.using_exceptions exceptions

compile_and_run debug test.coverage.scoped_enum store_error

compile_and_run debug test.coverage.shared store_error
compile_and_run debug test.coverage.shared.using_exceptions exceptions

compile_and_run debug test.coverage.shared_by_default store_error
compile_and_run debug test.coverage.shared_by_default.using_exceptions exceptions


cd unit_tests

gcov -a -l -f \
--object-directory ../obj/Debug/test.coverage \
--object-directory ../obj/Debug/test.coverage.using_exceptions \
--object-directory ../obj/Debug/test.coverage.string_is_integral \
--object-directory ../obj/Debug/test.coverage.string_is_integral.using_exceptions \
--object-directory ../obj/Debug/test.coverage.scoped_enum \
--object-directory ../obj/Debug/test.coverage.shared \
--object-directory ../obj/Debug/test.coverage.shared.using_exceptions \
--object-directory ../obj/Debug/test.coverage.shared_by_default/ \
--object-directory ../obj/Debug/test.coverage.shared_by_default.using_exceptions/ \
../src/*.cpp


cd ..

lcov \
--directory obj/Debug/test.coverage/ \
--directory obj/Debug/test.coverage.using_exceptions/ \
--directory obj/Debug/test.coverage.string_is_integral \
--directory obj/Debug/test.coverage.string_is_integral.using_exceptions \
--directory obj/Debug/test.coverage.scoped_enum/ \
--directory obj/Debug/test.coverage.shared/ \
--directory obj/Debug/test.coverage.shared.using_exceptions/ \
--directory obj/Debug/test.coverage.shared_by_default/ \
--directory obj/Debug/test.coverage.shared_by_default.using_exceptions/ \
--capture --output-file lcov_tmp.info -b ./unit_tests



lcov --extract lcov_tmp.info "$(pwd)/*" --output-file oolua.info
lcov --remove oolua.info "/unit_tests/*" "/usr/*" -o oolua.info
genhtml oolua.info --legend -o build_logs/coverage_report

rm oolua.info lcov_tmp.info
premake4 clean
cd dev

