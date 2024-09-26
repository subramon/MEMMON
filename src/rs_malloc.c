#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "q_macros.h"

#include "rs_malloc.h"
extern mmon_t g_mmon;

#define LUA_LIB
//-- START: Load as few libraries as possible
static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
//  {LUA_LOADLIBNAME, luaopen_package},
//  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
//  {LUA_MATHLIBNAME, luaopen_math},
//  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};

LUALIB_API void my_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}
//--  STOP: Load as few libraries as possible


int
rs_free(
    void *X
    )
{
  int status = 0;
  char lcmd[1024]; memset(lcmd, 0, 1024);  // commands to Lua 
  lua_State *L = g_mmon.L;
  size_t sz = 0;
  // Check if X is valid pointer. 
  sprintf(lcmd, "assert(Tmallocs[%" PRIu64 "])", (uint64_t)X);
  lexec(L, lcmd);  cBYE(status);
  lexec(L, " for k1, v1 in pairs(Tmallocs) do for k2, v2 in pairs(v1) do print(k1, k2, v2) end end "); cBYE(status);
  //-- Get the size of the memory pointed to by this pointer 
  // Put lua function get_size() on stack 
  int chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  lua_getglobal(L, "get_size");
  chk = lua_gettop(L); if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isfunction(L, -1)) {
    fprintf(stderr, "Lua Function get_size() undefined\n");
    lua_pop(L, 1);
    go_BYE(-1);
  }
  // Push argument to get_size() on stack 
  lua_pushnumber(L, (uint64_t)X);
  chk = lua_gettop(L); if ( chk != 2 ) { go_BYE(-1); }
  // call lua function and check status 
  status = lua_pcall(L, 1, 1, 0);
  if ( status != 0 ) {
    fprintf(stderr, "fn get_size() failed: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    go_BYE(-1); 
  }
  chk = lua_gettop(L); 
  if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isnumber(L, 1) ) { go_BYE(-1); } 
  sz = lua_tonumber(L, -1);
  if ( sz == 0 ) { go_BYE(-1); } 
  // clean up lua stack 
  lua_pop(L, 1);
  chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  //---------------------------------
  g_mmon.num_free++;
  g_mmon.sz_free += sz;
  if ( g_mmon.sz_free > g_mmon.sz_malloc ) { go_BYE(-1); }
  free(X);
BYE:
  return status;
}

void *
rs_malloc(
    size_t sz,
    const char * const file,
    int line,
    const char * const func,
    const char * const label
    )
{
  int status =  0;
  char lcmd[1024]; memset(lcmd, 0, 1024);  // commands to Lua 
  lua_State *L = g_mmon.L;

  void *X = NULL;
  if ( sz == 0 ) { go_BYE(-1); }
  X = malloc(sz); 
  if ( X == NULL ) { go_BYE(-1); }
  g_mmon.num_malloc++;
  g_mmon.sz_malloc += sz;
  sprintf(lcmd, "assert(not Tmallocs[%" PRIu64 "])", (uint64_t)X);
  lexec(L, lcmd);  cBYE(status);
  sprintf(lcmd, "t = {}"); 
  lexec(L, lcmd); 
  sprintf(lcmd, "t.size = %" PRIu64 "", sz); 
  lexec(L, lcmd); 
  sprintf(lcmd, "t.file = \"%s\"", file);
  lexec(L, lcmd); 
  sprintf(lcmd, "t.line = %d", line);
  lexec(L, lcmd); 
  sprintf(lcmd, "t.func = \"%s\"", func);
  lexec(L, lcmd); 
  sprintf(lcmd, "Tmallocs[%" PRIu64 "] = t", (uint64_t)X);
  lexec(L, lcmd); 
  sprintf(lcmd, "t = nil"); 
  lexec(L, lcmd); 
  /*
  sprintf(lcmd, "for k1, v1 in pairs(Tmallocs) do "
      "for k2, v2 in pairs(v1) do print(k1, k2, v2) end end ");
  lexec(L, lcmd); 
  */
BYE:
  if ( status == 0 ) { return X; } else { return NULL; } 
}

int 
init_mmon(
    mmon_t *ptr_M
    )
{
  int status = 0;
  lua_State *L = NULL;
  memset(ptr_M, 0, sizeof(mmon_t));
  L = luaL_newstate(); if ( L == NULL ) { go_BYE(-1); }
  my_openlibs(L);
  char lcmd[1024]; memset(lcmd, 0, 1024);  // commands to Lua 

  strcpy(lcmd, "Tmallocs = {}");
  lexec(L, lcmd); cBYE(status); 

  strcpy(lcmd, " get_size = function(x) ");
  strcat(lcmd, "   assert(type(x) == \"number\")");
  strcat(lcmd, "   local t = assert(Tmallocs[x])");
  strcat(lcmd, "   assert(type(t) == \"table\")");
  strcat(lcmd, "   local sz = assert(t.size)");
  strcat(lcmd, "   assert(type(sz) == \"number\")");
  strcat(lcmd, "   return sz");
  strcat(lcmd, " end");
  lexec(L, lcmd); cBYE(status);
  lexec(L, "assert(type(get_size) == \"function\")"); cBYE(status);
  // printf("INIT DONE \n");

  int chk = lua_gettop(L); 
  ptr_M->L =L;
BYE:
  return status;
}

int 
chck_mmon(
    mmon_t *ptr_M
    )
{
  int status = 0;
  if ( ptr_M == NULL ) { go_BYE(-1); }
  if ( ptr_M->L == NULL ) { go_BYE(-1); }
  if ( ptr_M->sz_malloc !=ptr_M->sz_free ) { go_BYE(-1); }
BYE:
  return status;
}

int 
prnt_mmon(
    mmon_t *ptr_M
    )
{
  int status = 0;
  if ( ptr_M == NULL ) { go_BYE(-1); }
  fprintf(stdout, "Malloc bytes = %" PRIu64 "\n", ptr_M->sz_malloc);
  fprintf(stdout, "Free   bytes = %" PRIu64 "\n", ptr_M->sz_free);
  fprintf(stdout, "Malloc count = %" PRIu64 "\n", ptr_M->num_malloc);
  fprintf(stdout, "Free   count = %" PRIu64 "\n", ptr_M->num_free);

BYE:
  return status;
}

int 
free_mmon(
    mmon_t *ptr_M
    )
{
  int status = 0;
  if ( ptr_M == NULL ) { go_BYE(-1); }
  if ( ptr_M->L != NULL ) {
    lua_close(ptr_M->L);
  }
  memset(ptr_M, 0, sizeof(mmon_t));
BYE:
  return status;
}
