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
  status = foo(); cBYE(status);
  status = chck_mmon(&g_mmon); cBYE(status);
  status = prnt_mmon(&g_mmon); cBYE(status);
  status = free_mmon(&g_mmon); cBYE(status);
  printf("SUCCESS\n");
BYE:
  return status;
}

