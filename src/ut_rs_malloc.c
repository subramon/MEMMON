/*
gcc -g -std=gnu99 ut_rs_malloc.c ../../RSUTILS/src/cat_to_buf.c rs_malloc.c -I../..//RSUTILS/inc/   /usr/local/lib/libluajit-5.1.so -I../inc/ -I../../RSUTILS/inc/
 */
#include <stdio.h>
#include <stdlib.h>
#include "q_macros.h"
#include "rs_malloc.h"

mmon_t g_mmon;

#define USE_MEMMON
#ifdef USE_MEMMON
#define malloc(x) { rs_malloc(x, __FILE__, __LINE__, __FUNCTION__, "") }
#define free(x)   { rs_free(x); }
#endif
  
int
foo(
    )
{
  int status = 0;
  char *X = malloc(16); 
  free(X);
  return status;
}

int
main(
  int argc,
  char **argv
    )
{
  int status = 0;
  status = init_mmon(&g_mmon); cBYE(status);
  for ( int i = 0; i < 10; i++ ) { 
  status = foo(); cBYE(status);
  }
  status = chck_mmon(&g_mmon); cBYE(status);
  status = prnt_mmon(&g_mmon); cBYE(status);
  status = dump_mmon(&g_mmon, "_mmon.json"); cBYE(status);
  status = stat_mmon(&g_mmon, "_stat.json"); cBYE(status);
  status = free_mmon(&g_mmon); cBYE(status);
  printf("SUCCESS\n");
BYE:
  return status;
}

