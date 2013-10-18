#!/bin/bash 

function failed()
{
    echo "Failed: $@" >&2
    exit 1
}


set -e

cd ..
premake4 clean
premake4 gmake linux > /dev/null

if [ ! -d build_logs ]; then
	mkdir build_logs
fi

rm -rf build_logs/coverage_report


#cd unit_tests
echo "building Test Coverage | Debug | store error : "
make config=debug test.coverage >  ./build_logs/test.coverage_debug_store_error.log || failed debug build;
echo "build passed"
echo "building Test Coverage | Release | exceptions | string is integral : "
make config=release test.coverage >  ./build_logs/test.coverage_release_exceptions.log || failed release build;
echo "build passed"
#cd ..

cd unit_tests
gcov -a -l -f --object-directory ../obj/Debug/test.coverage --object-directory ../obj/Release/test.coverage ../src/*.cpp
cd ..

lcov --directory obj/Debug/test.coverage/ --directory obj/Release/test.coverage/ --capture --output-file lcov_tmp.info -b ./unit_tests
lcov --extract lcov_tmp.info "$(pwd)/*" --output-file oolua.info
genhtml oolua.info -o build_logs/coverage_report

rm oolua.info lcov_tmp.info
premake4 clean
cd dev

