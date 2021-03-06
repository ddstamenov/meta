#!/usr/bin/env bash

# tiny build script

FULL_SCRIPT=`readlink -f $0`
DIRNAME=`dirname $FULL_SCRIPT`
SCRIPT=`basename $FULL_SCRIPT`

cd $DIRNAME

BUILD_DIR=build
mkdir -p $BUILD_DIR 

CLANG_CXX=clang++
GCC_CXX=g++

DEBUG_OPT="-ggdb3"
RELESE_OPT="-O2"

CLANG_ASAN="-fsanitize=address -fno-omit-frame-pointer"

BUILD_TYPE_OPT=$DEBUG_OPT
# BUILD_TYPE_OPT=$RELESE_OPT

INCLUDE="-I `pwd`/../include -I `pwd`"

FILES=(`ls *.cpp`)
if [ ${#FILES[@]} -eq 0 ]
then
   echo "No cpp in directory `pwd`"
   exit 1
fi

# using clang-static-analyzer
if [ -z $CXX ] ; then
   CXX=$CLANG_CXX
fi

BUILD_OK=0

for FILE in "${FILES[@]}"
do
   OUTPUT=`echo $FILE | sed -e 's/\.cpp//'`
   INPUT_FILE="${OUTPUT}.cpp"

   echo "Compile $INPUT_FILE ($CLANG_CXX) ..."
   CMD="$CXX -std=c++14 $INCLUDE $BUILD_TYPE_OPT $INPUT_FILE $CLANG_ASAN \
      -o $BUILD_DIR/${OUTPUT}_clang"
   echo $CMD
   $CMD
   ret=$?
   if [ $ret -ne 0 ]
   then
      BUILD_OK=$ret
   fi

   echo "Compile $INPUT_FILE ($GCC_CXX) ..."
   CMD="$GCC_CXX -std=c++14 $INCLUDE $BUILD_TYPE_OPT $INPUT_FILE \
      -o $BUILD_DIR/${OUTPUT}_gcc"
   echo $CMD
   $CMD
   ret=$?
   if [ $ret -ne 0 ]
   then
      BUILD_OK=$ret
   fi
done

# --------------------------------------------------------------------------------
# Build single demo test binary using multiple sources
DEMODIR=demo-test-framework
cd $DEMODIR
BUILD_DIR="../build"
INCLUDE="-I `pwd`/../../include -I `pwd`"

FILES=`ls *.cpp`
echo "compile file: $FILES"

OUTPUT=multy_cpp_bin

echo "Compile $FILES ($CLANG_CXX) ..."
CMD="$CXX -std=c++14 $INCLUDE $BUILD_TYPE_OPT $FILES $CLANG_ASAN \
   -o $BUILD_DIR/${OUTPUT}_clang"
echo $CMD
$CMD
ret=$?
if [ $ret -ne 0 ]
then
   BUILD_OK=$ret
fi

echo "Compile $FILES ($GCC_CXX) ..."
CMD="$GCC_CXX -std=c++14 $INCLUDE $BUILD_TYPE_OPT $FILES -o $BUILD_DIR/${OUTPUT}_gcc"
echo $CMD
$CMD
ret=$?
if [ $ret -ne 0 ]
then
   BUILD_OK=$ret
fi

exit $BUILD_OK
