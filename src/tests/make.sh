#!/usr/bin/env bash

# tiny build script

CXX=clang++
#CXX=g++

DEBUG_OPT="-g"
RELESE_OPT="-O2"

INCLUDE="-I `pwd`/../include -I `pwd`"

FILES=( \
   identity \
   )

for FILE in "${FILES[@]}"
do
   OUTPUT=$FILE
   INPUT_FILE="${OUTPUT}Tests.cpp"
   echo "Compile $INPUT_FILE ..."

   CMD="$CXX -std=c++14 $INCLUDE $DEBUG_OPT $INPUT_FILE -o $OUTPUT"
   echo $CMD
   $CMD
done

