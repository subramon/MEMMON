#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "q_macros.h"
#include "file_as_str.h"

#include "rs_malloc.h"
extern mmon_t g_mmon;

#define LUA_LIB
//-- START: Load as few libraries as possible
static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_STRLIBNAME, luaopen_io},
  {LUA_MATHLIBNAME, luaopen_math},
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
//-------------------------------------------------------
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

  const char *lua_file = "malloc_free_recorder.lua"; // TODO hard coded

  char *lcmd = file_as_str(lua_file); 
  if ( lcmd == NULL ) { go_BYE(-1); }
  mcr_lexec(L, lcmd); cBYE(status); 

  int chk = lua_gettop(L); 
  if ( chk != 0 ) { go_BYE(-1); }
  ptr_M->L =L;
BYE:
  free_if_non_null(lcmd);
  return status;
}
//-------------------------------------------------------
int 
chck_mmon(
    mmon_t *ptr_M
    )
{
  int status = 0;
  if ( ptr_M == NULL ) { go_BYE(-1); }
  if ( ptr_M->L == NULL ) { go_BYE(-1); }
  if ( ptr_M->sz_malloc < 0 ) { go_BYE(-1); }
  if ( ptr_M->sz_malloc < ptr_M->sz_free ) { go_BYE(-1); }
BYE:
  return status;
}
//-------------------------------------------------------
int 
stat_mmon(
    mmon_t *ptr_M,
    const char * const file_name
    )
{
  int status = 0;
  char *buf = NULL; uint32_t sz = 0, len = 0;
  char lbuf[128];
  FILE *fp = NULL; 
  if ( ptr_M == NULL ) { go_BYE(-1); }
  if ( ( file_name == NULL ) || ( *file_name == '\0' ) ) { 
    fp = stdout;
  }
  else {
    fp = fopen(file_name, "w"); 
    return_if_fopen_failed(fp,  file_name, "w"); 
  }

  status = cat_to_buf(&buf, &sz, &len, "{", 0); cBYE(status);

  sprintf(lbuf, "\"sz_malloc\" : %" PRIi64 ", ", ptr_M->sz_malloc); 
  status = cat_to_buf(&buf, &sz, &len, lbuf, 0); cBYE(status);

  sprintf(lbuf, "\"sz_free\" : %" PRIi64 ", ", ptr_M->sz_free); 
  status = cat_to_buf(&buf, &sz, &len, lbuf, 0); cBYE(status);

  sprintf(lbuf, "\"num_malloc\" : %" PRIi64 ", ", ptr_M->num_malloc); 
  status = cat_to_buf(&buf, &sz, &len, lbuf, 0); cBYE(status);

  sprintf(lbuf, "\"num_free\" : %" PRIi64 "", ptr_M->num_free); 
  status = cat_to_buf(&buf, &sz, &len, lbuf, 0); cBYE(status);

  status = cat_to_buf(&buf, &sz, &len, "}", 0); cBYE(status);
  fprintf(fp, "%s\n", buf);


BYE:
  if ( file_name != NULL ) { 
    fclose_if_non_null(fp);
  }
  free_if_non_null(buf);
  return status;
}
//-------------------------------------------------------
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
//-------------------------------------------------------
int 
dump_mmon(
    mmon_t *ptr_M,
    const char * const file_name
    )
{
  int status = 0;
  if ( ptr_M == NULL ) { go_BYE(-1); }
  if ( file_name == NULL ) { go_BYE(-1); }
  lua_State *L = ptr_M->L;

  // Put lua function dump_mmon() on stack 
  int chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  lua_getglobal(L, "dump_mmon");
  chk = lua_gettop(L); if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isfunction(L, -1)) {
    fprintf(stderr, "Lua Function dump_mmon() undefined\n");
    lua_pop(L, 1);
    go_BYE(-1);
  }
  // Push argument to dump_mmon() on stack 
  lua_pushstring(L, file_name);
  chk = lua_gettop(L); if ( chk != 2 ) { go_BYE(-1); }
  // call lua function and check status 
  status = lua_pcall(L, 1, 1, 0);
  if ( status != 0 ) {
    fprintf(stderr, "fn dump_mmon() failed: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    go_BYE(-1); 
  }
  chk = lua_gettop(L); 
  if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isboolean(L, 1) ) { go_BYE(-1); } 
  bool bstatus = lua_toboolean(L, -1);
  if ( !bstatus ) { WHEREAMI; status = -1; } 
  // clean up lua stack 
  lua_pop(L, 1);
  chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  //---------------------------------
BYE:
  return status;
}
//-------------------------------------------------------
int
rs_free(
    void *X
    )
{
  int status = 0;
  char lcmd[1024]; memset(lcmd, 0, 1024);  // commands to Lua 
  lua_State *L = g_mmon.L;
  size_t sz = 0;
  // Put lua function record_free() on stack 
  int chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  lua_getglobal(L, "record_free");
  chk = lua_gettop(L); if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isfunction(L, -1)) {
    fprintf(stderr, "Lua Function record_free() undefined\n");
    lua_pop(L, 1);
    go_BYE(-1);
  }
  // Push argument to record_free() on stack 
  lua_pushnumber(L, (uint64_t)X);
  chk = lua_gettop(L); if ( chk != 2 ) { go_BYE(-1); }
  // call lua function and check status 
  status = lua_pcall(L, 1, 1, 0);
  if ( status != 0 ) {
    fprintf(stderr, "fn record_free() failed: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    go_BYE(-1); 
  }
  chk = lua_gettop(L); 
  if ( chk != 1 ) { go_BYE(-1); }
  //-- Get the size of the memory pointed to by this pointer 
  if ( !lua_isnumber(L, 1) ) { go_BYE(-1); } 
  sz = lua_tonumber(L, -1);
  if ( sz <= 0 ) { go_BYE(-1); } 
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
    size_t size,
    const char * const file,
    int line,
    const char * const func
    )
{
  int status =  0;
  char lcmd[1024]; memset(lcmd, 0, 1024);  // commands to Lua 
  lua_State *L = g_mmon.L;

  void *X = NULL;
  if ( size == 0 ) { go_BYE(-1); }
  X = malloc(size); 
  if ( X == NULL ) { go_BYE(-1); }
  g_mmon.num_malloc++;
  g_mmon.sz_malloc += size;
  // Update Lua data structure
  // Put lua function record_malloc() on stack 
  int chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  lua_getglobal(L, "record_malloc");
  chk = lua_gettop(L); if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isfunction(L, -1)) {
    fprintf(stderr, "Lua Function record_malloc() undefined\n");
    lua_pop(L, 1);
    go_BYE(-1);
  }
  // Push arguments to record_malloc() on stack 
  lua_pushnumber(L, (uint64_t)X);
  lua_pushnumber(L, size);
  lua_pushstring(L, file);
  lua_pushnumber(L, line);
  lua_pushstring(L, func);
  chk = lua_gettop(L); if ( chk != 6 ) { go_BYE(-1); }
  // call lua function and check status 
  status = lua_pcall(L, 5, 1, 0);
  if ( status != 0 ) {
    fprintf(stderr, "fn record_malloc() failed: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    go_BYE(-1); 
  }
  chk = lua_gettop(L); 
  if ( chk != 1 ) { go_BYE(-1); }
  if ( !lua_isboolean(L, 1) ) { go_BYE(-1); } 
  bool bstatus = lua_toboolean(L, -1);
  if ( !bstatus ) { WHEREAMI; status = -1; } 
  // clean up lua stack 
  lua_pop(L, 1);
  chk = lua_gettop(L); if ( chk != 0 ) { go_BYE(-1); }
  //---------------------------------
BYE:
  if ( status == 0 ) { return X; } else { return NULL; } 
}
