#!/bin/bash

FULL_SCRIPT=`readlink -f $0`
DIRNAME=`dirname $FULL_SCRIPT`
SCRIPT=`basename $FULL_SCRIPT`

cd $DIRNAME
cd ../src

HEADERS=(`find . -name "*.h" -type f`)

echo "======= processing header files "
for HEADER in ${HEADERS[@]}
do
   echo "clang-format for: $HEADER"
   clang-format -i $HEADER
done

SOURCES=(`find . -name "*.cpp" -type f`)

echo "======= processing cpp files "
for SOURCE in ${SOURCES[@]}
do
   echo "clang-format for: $SOURCE"
   clang-format -i $SOURCE
done


# | xargs clang-format -i

