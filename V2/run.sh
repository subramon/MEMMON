#!/bin/bash
set -e
export RSUTILS_SRC_ROOT=$HOME/RSUTILS/
gcc -g -Wall -std=gnu99 ut_rs_malloc.c rs_malloc.c \
  -I../inc/ -I${RSUTILS_SRC_ROOT}/inc/ \
  /usr/local/lib/libluajit-5.1.so ${RSUTILS_SRC_ROOT}/src/librsutils.so

valgrind ./a.out 1>_x 2>&1
grep "definitely lost: 0 bytes in 0 blocks" _x 1>/dev/null 2>&1

rm -f _* a.out
echo "SUCCESS for $0 in $PWD"
