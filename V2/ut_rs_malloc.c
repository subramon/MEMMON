/*
gcc -g -Wall -std=gnu99 ut_rs_malloc.c rs_malloc.c \
  -I../inc/ -I${RSUTILS_SRC_ROOT}/inc/ \
  /usr/local/lib/libluajit-5.1.so ${RSUTILS_SRC_ROOT}/src/librsutils.so
 */
#include <stdio.h>
#include <stdlib.h>
#include "q_macros.h"
#include "rs_malloc.h"

mmon_t g_mmon;

#define USE_MEMMON
#ifdef USE_MEMMON
#define free(x) ( rs_free(x, __FUNCTION__) )
#define malloc(x) ( rs_malloc(x, __FILE__, __LINE__, __FUNCTION__) )
#endif
  
void *
foo(
    int n
    )
{
  char *X = malloc(n); 
  return X;
}

void
bar(
    void *X
   )
{
  free(X);
}

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
#define N 10
  void *X[N];
  status = init_mmon(&g_mmon); cBYE(status);
  status = chck_mmon(&g_mmon); cBYE(status);
  status = dump_mmon(&g_mmon, "_init_mmon.json"); cBYE(status);
  status = stat_mmon(&g_mmon,NULL); cBYE(status);
  for ( int i = 0; i < N; i++ ) { 
    X[i] = foo(16);  if ( X[i] == NULL ) { go_BYE(-1); }
    char fname[32]; sprintf(fname, "_%d_foo.json", i); 
    status = dump_mmon(&g_mmon, fname); cBYE(status);
  }
  for ( int i = 0; i < 10; i++ ) { 
    bar(X[i]); 
    char fname[32]; sprintf(fname, "_%d_bar.json", i); 
    status = dump_mmon(&g_mmon, fname); cBYE(status);
  }
  status = chck_mmon(&g_mmon); cBYE(status);
  status = stat_mmon(&g_mmon, ""); cBYE(status);
  status = chck_mmon(&g_mmon); cBYE(status);
  status = pr_malloc_free_per_func("foo");
  status = pr_malloc_free_per_func("bar");
  status = free_mmon(&g_mmon); cBYE(status);

  printf("SUCCESS\n");
BYE:
  return status;
}

