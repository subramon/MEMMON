#ifndef _RS_MALLOC_H
#define  _RS_MALLOC_H
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <stdint.h>

typedef struct _mmon_t {
  lua_State *L;
  uint64_t sz_malloc;
  uint64_t sz_free;
  uint64_t num_malloc;
  uint64_t num_free;
} mmon_t;

#define malloc(x) { rs_malloc(x, __FILE__, __LINE__, __FUNCTION__) }
#define free(x)   { rs_free(x); }

// Following macro for brevity
#define lexec(L, s) { \
  status = luaL_dostring(L, s); \
  if ( status != 0 ) {  \
    fprintf(stderr, "Failed to execute [%s] \n", s); 
    fprintf(stderr, "Error = %s\n", lua_tostring(L,-1)); \
  } \
}

extern int
rs_free(
    void *X
    );
extern void *
rs_malloc(
    size_t sz,
    const char * const file,
    int line,
    const char * const func
    );
extern int 
init_mmon(
    mmon_t *ptr_M
    );
extern int 
chck_mmon(
    mmon_t *ptr_M
    );
extern int 
prnt_mmon(
    mmon_t *ptr_M
    );
extern int 
free_mmon(
    mmon_t *ptr_M
    );
#endif //  _RS_MALLOC_H
