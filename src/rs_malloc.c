#include <stdio.h>
#include <stdlib.h>

extern lua_State *g_mem_L;
void *
rs_malloc(
    size_t size
    )
{
  int status =  0;
  void *X = NULL;
BYE:
  if ( status == 0 ) { return X; } else { return NULL; } 

}
