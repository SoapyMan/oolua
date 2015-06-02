#!/bin/bash
#1: gnu or xcode
#2: full semantic version i.e 2.0.0
./generate_doc.sh ${1} ${2}
./oolua_generate_download.sh ${2}
