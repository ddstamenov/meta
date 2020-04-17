#!/usr/bin/env bash

# tiny build script

FULL_SCRIPT=`readlink -f $0`
DIRNAME=`dirname $FULL_SCRIPT`
SCRIPT=`basename $FULL_SCRIPT`

BUILD_DIR=build
cd $DIRNAME/$BUILD_DIR

TESTS=(`ls`)

COUNT=0
for TEST in "${TESTS[@]}"
do
   echo "----------- Running: $TEST ..."
   ./$TEST
   let COUNT++
done


echo -e "\n===================(summary)================\nRun $COUNT test files"
