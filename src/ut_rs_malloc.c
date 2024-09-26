#include <stdio.h>
#include "q_macros.h"
#include "rs_malloc.h"

mmon_t g_mmon;

int
foo(
    )
{
  int status = 0;
  char *X = Xmalloc(16, "label");
  rs_free(X);
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

