#ifndef _RS_MALLOC_H
#define  _RS_MALLOC_H
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <stdint.h>
#include "cat_to_buf.h"

typedef struct _mmon_t {
  lua_State *L;
  uint64_t sz_malloc;
  uint64_t sz_free;
  uint64_t num_malloc;
  uint64_t num_free;
} mmon_t;

#define Xmalloc(x, y) { \
  rs_malloc(x, __FILE__, __LINE__, __FUNCTION__, y) \
}
// Following macro for brevity
#define lexec(L, s) { \
  status = luaL_dostring(L, s); \
  if ( status != 0 ) {  \
    fprintf(stderr, "Error luaL_string=%s\n", lua_tostring(L,-1)); \
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
    const char * const func,
    const char * const label
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
extern int 
dump_mmon(
    mmon_t *ptr_M,
    const char * const file_name
    );
extern int 
stat_mmon(
    mmon_t *ptr_M,
    const char * const file_name
    );
#endif //  _RS_MALLOC_H
