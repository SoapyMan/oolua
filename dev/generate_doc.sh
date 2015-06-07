#!/bin/bash
#1: gnu or xcode
#2: full semantic version i.e 2.0.0
sudo ./sudo_library_compare.sh $1
cd ../build_logs
lua51 '../dev/profile_summary.lua' > "profile_summary.txt"
cd ..
#/Applications/Doxygen.app/Contents/Resources/doxygen dev/oolua.Doxyfile
/usr/local/bin/doxygen ./dev/oolua.Doxyfile

if [ ! -d ../oolua_docs ]; then
	rm -f -r ../oolua_docs
fi
mkdir ../oolua_docs
mkdir ../oolua_docs/html
mkdir ../oolua_docs/latex

/usr/bin/doxygen ./dev/oolua.Doxyfile


cwd=$(pwd)
cd ../oolua_docs/latex
pdflatex refman
makeindex refman.idx
pdflatex refman
cp refman.pdf $cwd/dev/oolua-${2}.pdf
cd $cwd
cd ./dev
