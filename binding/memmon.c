// Purpose is to be able to track malloc/free 
#define LUA_LIB

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "luaconf.h"
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

int luaopen_libmemmon (lua_State *L);

//----------------------------------------
static int l_memmon_malloc( 
    lua_State *L
    )
{
  int status = 0;
  if ( ( nargs < 1 ) || ( nargs > XXX ) ) { go_BYE(-1); }

  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *pcmd  = luaL_checkstring(L, 1);
  if ( ( pcmd == NULL ) ||( *pcmd == '\0' ) ) { go_BYE(-1); }

  // create output file 
  temp_file_name = strdup("/tmp/_memmon_XXXXXX"); 
  fd = mkstemp(temp_file_name); 
  if ( fd < 0 ) { go_BYE(-1); }

  ifp = popen(pcmd, "r");
  if ( ifp == NULL ) { go_BYE(-1); }
  for ( ; ; ) { 
    memset(buf, 0, BUFLEN);
    ssize_t nr = fread(buf, 1, BUFLEN-1, ifp); 
    if ( nr < 0 ) { go_BYE(-1); }
    if ( nr == 0 ) { break; }
    ssize_t nw = write(fd, buf, nr); 
    if ( nw != nr ) { go_BYE(-1); }
    if ( nr < BUFLEN-1 ) { break; }
  }
  pclose(ifp);  ifp = NULL; 
  close(fd); fd = -1;
  char *str = file_as_str(temp_file_name); 
  if ( temp_file_name != NULL ) { unlink(temp_file_name); } 
  free_if_non_null(temp_file_name); 
  lua_pushstring(L, str); 
  free(str); // Lua has taken control TODO P0 THINK 
  return 1; 
BYE:
  if ( fd >= 0 ) { close(fd); }
  if ( temp_file_name != NULL ) { unlink(temp_file_name); } 
  free_if_non_null(temp_file_name); 
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_basename( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *path = luaL_checkstring(L, 1);
  char *x = strdup(path);
  const char *base = basename(x);
  lua_pushstring(L, base);
  free_if_non_null(x);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_date_str_to_epoch( 
    lua_State *L
    )
{
  int status = 0;
  time_t t_epoch;
  if ( lua_gettop(L) != 2 ) { go_BYE(-1); }
  const char *date_str = luaL_checkstring(L, 1);
  const char *format   = luaL_checkstring(L, 2);
  //------------------
  struct tm l_tm; memset(&l_tm, 0, sizeof(struct tm));
  char *rslt = strptime(date_str, format, &l_tm);
  if ( rslt == NULL ) { go_BYE(-1); }
  t_epoch = tm2time(&l_tm);
  //------------------
  lua_pushnumber(L, t_epoch);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
//----------------------------------------
static int l_memmon_dirname( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *path = luaL_checkstring(L, 1);
  char *x = strdup(path);
  const char *dir = dirname(x);
  lua_pushstring(L, dir);
  free_if_non_null(x);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_qtypes( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 0 ) { go_BYE(-1); }
  char *str = qtypes_as_lua_tbl(); if ( str == NULL ) { go_BYE(-1); }
  lua_pushstring(L, str);
  free_if_non_null(str); // TODO Check 
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_realpath( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *path = luaL_checkstring(L, 1);
  char *str_realpath = realpath(path, NULL);
  if ( str_realpath == NULL ) { go_BYE(-1); } 
  if ( !isfile(str_realpath) ) { go_BYE(-1); } 

  lua_pushstring(L, str_realpath);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_rand_file_name( 
    lua_State *L
    )
{
  int status = 0;
  char buf[64]; memset(buf, 0, 64);
  status = rand_file_name(buf, 63);
  lua_pushstring(L, buf);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3; 
}
//----------------------------------------
static int l_memmon_rdtsc( 
    lua_State *L
    )
{
  uint64_t x = (uint64_t)RDTSC();
  lua_pushnumber(L, x);
  return 1;
}
//----------------------------------------
static int l_memmon_get_bit_u64( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 2 ) { go_BYE(-1); }
  const uint64_t *X  = (const uint64_t *)lua_topointer(L, 1);
  uint64_t bnum = luaL_checknumber(L, 2);
  if ( bnum >= 64 ) { go_BYE(-1); }
  int bval = get_bit_u64(X, bnum); 
  lua_pushnumber(L, bval);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
static int l_memmon_is_qtype( 
    lua_State *L
    )
{
  const char *const str_qtype = luaL_checkstring(L, 1);
  lua_pushboolean(L, is_qtype(str_qtype));
  return 1;
}
//----------------------------------------
static int l_memmon_isdir( 
    lua_State *L
    )
{
  const char *const dir = luaL_checkstring(L, 1);
  bool exists = isdir(dir);
  lua_pushboolean(L, exists);
  return 1;
}
//----------------------------------------
static int l_memmon_num_cols( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *const file_name = luaL_checkstring(L, 1);
  int nc = num_cols(file_name); if ( nc < 0 ) { go_BYE(-1); }
  lua_pushnumber(L, nc);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
static int l_memmon_num_lines( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *const file_name = luaL_checkstring(L, 1);
  int nl = num_lines(file_name, NULL, 0); if ( nl < 0 ) { go_BYE(-1); }
  lua_pushnumber(L, nl);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_memmon_omp_get_num_procs( 
    lua_State *L
    )
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  if ( lua_gettop(L) != 0 ) { go_BYE(-1); }
  lua_pushnumber(L, omp_get_num_procs());
  return 1; 
BYE:
  mcr_rs_munmap(X, nX);
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
static int l_memmon_mkstemp( 
    lua_State *L
    )
{
  int status = 0;
  const char *const template = luaL_checkstring(L, 1);
  if ( template == NULL ) { go_BYE(-1); } 
  char * temp_file_name = strdup(template); 
  int fd = mkstemp(temp_file_name); 
  if ( fd < 0 ) { go_BYE(-1); }
  close(fd); 
  lua_pushstring(L, temp_file_name);
  free_if_non_null(temp_file_name);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
static int l_memmon_line_breaks( 
    lua_State *L
    )
{
  int status = 0;

  int nargs = lua_gettop(L);
  if ( nargs != 2 ) { go_BYE(-1); }
  const char *const infile = luaL_checkstring(L, 1);
  const char *const outfile = luaL_checkstring(L, 2);
  status = line_breaks(infile, outfile); cBYE(status);
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_makepath( 
    lua_State *L
    )
{
  int status = 0;
  const char *const dir_name = luaL_checkstring(L, 1);
  if ( !isdir(dir_name) ) { 
    // read/write/search permissions for owner and group, 
    // read/search permissions for others.
    status = mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if ( status < 0 ) { 
    fprintf(stderr, "Failed to make directory [%s]\n", dir_name);
    }
    cBYE(status);
  }
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_mk_file( 
    lua_State *L
    )
{
  int status = 0;
  int nargs = lua_gettop(L);
  if ( ( nargs < 3 ) || ( nargs > 4 ) ) { go_BYE(-1); }
  const char *const dir_name  = luaL_checkstring(L, 1);
  const char *const file_name = luaL_checkstring(L, 2);
  uint64_t file_size = luaL_checknumber(L, 3); 
  bool over_write = false;
  if ( nargs == 4 ) { 
    over_write = lua_toboolean(L, 4);
  }
  //--------------------------------------
  if ( file_exists(file_name) ) {
    if ( over_write ) {
      unlink(file_name);
    }
    else {
      fprintf(stderr, "File %s exists \n", file_name);
      go_BYE(-1);
    }
  }
  //--------------------------------------
  if ( *dir_name == '\0' )  {
    status = mk_file(NULL, file_name, file_size);
  }
  else {
    status = mk_file(dir_name, file_name, file_size);
  }
  cBYE(status);
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_unlink( 
    lua_State *L
    )
{
  int status = 0;
  const char *const file_name = luaL_checkstring(L, 1);
  status = unlink(file_name); cBYE(status);
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_mem_info( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 0 ) { go_BYE(-1); }
  size_t pagesize = 0; int64_t bytes = 0; size_t pages = 0;
  status = mem_info(&pagesize, &bytes, &pages); cBYE(status);
  lua_pushnumber(L, pagesize);
  lua_pushnumber(L, bytes);
  lua_pushnumber(L, pages);
  return 3;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_str_as_file( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 2 ) { go_BYE(-1); }
  const char *const str       = luaL_checkstring(L, 1);
  const char *const file_name = luaL_checkstring(L, 2);
  status = str_as_file(str, file_name); cBYE(status);
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
static int l_memmon_file_as_str( 
    lua_State *L
    )
{
  int status = 0;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  const char *const file_name = luaL_checkstring(L, 1);
  char *x  =  file_as_str(file_name);
  if ( x == NULL ) { go_BYE(-1); }
  lua_pushstring(L, x); 
  free(x); // Lua has taken control. TODO P0 Thinkl
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------
//----------------------------------------
static int l_memmon_getsize( 
    lua_State *L
    )
{
  const char *const file_name = luaL_checkstring(L, 1);
  int64_t file_size =  get_file_size(file_name);
  lua_pushnumber(L, file_size);
  return 1;
}
//----------------------------------------
static int l_memmon_isfile( 
    lua_State *L
    )
{
  const char *const file_name = luaL_checkstring(L, 1);
  bool exists = isfile(file_name);
  lua_pushboolean(L, exists);
  return 1;
}
//----------------------------------------
static int l_memmon_isfile_in_dir( 
    lua_State *L
    )
{
  int status = 0;
  int nargs = lua_gettop(L);
  if ( nargs != 2 ) { go_BYE(-1); }
  const char *const file_name = luaL_checkstring(L, 1);
  const char *const  dir_name = luaL_checkstring(L, 2);
  bool exists = isfile_in_dir(file_name, dir_name);
  lua_pushboolean(L, exists);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushnumber(L, status);
  lua_pushstring(L, __func__);
  return 3;
}
//----------------------------------------
static int l_memmon_str_qtype_to_str_ispctype( 
    lua_State *L
    )
{
  const char *const str_qtype = luaL_checkstring(L, 1);
  const char * const x = str_qtype_to_str_ispctype(str_qtype);
  if ( x == NULL ) { 
    lua_pushnil(L);
  }
  else { 
    lua_pushstring(L, x);
  }
  return 1;
}
//----------------------------------------
static int l_memmon_str_qtype_to_str_ctype( 
    lua_State *L
    )
{
  const char *const str_qtype = luaL_checkstring(L, 1);
  const char * const x = str_qtype_to_str_ctype(str_qtype);
  if ( x == NULL ) { 
    lua_pushnil(L);
  }
  else { 
    lua_pushstring(L, x);
  }
  return 1;
}
//----------------------------------------
static int l_memmon_get_width_qtype( 
    lua_State *L
    )
{
  const char *const str_qtype = luaL_checkstring(L, 1);
  int width = get_width_qtype(str_qtype);
  lua_pushnumber(L, width);
  return 1;
}
//----------------------------------------
static int l_memmon_get_c_qtype( 
    lua_State *L
    )
{
  const char *const str_qtype = luaL_checkstring(L, 1);
  qtype_t qtype  = get_c_qtype(str_qtype);
  lua_pushnumber(L, qtype);
  return 1;
}
//----------------------------------------
static int l_memmon_gettime( 
    lua_State *L
    )
{
  int status = 0;
  struct stat st_buf;
  double sec, nsec;
  const char *const file_name = luaL_checkstring(L, 1);
  status = stat(file_name, &st_buf); cBYE(status);

  const char *const str_mode = luaL_checkstring(L, 2);
  if ( strcmp(str_mode, "last_access") == 0 ) { 
    sec =  st_buf.st_atim.tv_sec;
    nsec = st_buf.st_atim.tv_nsec;
  }
  else if ( strcmp(str_mode, "last_mod") == 0 ) { 
    sec =  st_buf.st_mtim.tv_sec;
    nsec = st_buf.st_mtim.tv_nsec;
  }
  else {
    go_BYE(-1);
  }
  lua_pushnumber(L, sec);
  lua_pushnumber(L, nsec);
  return 2; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_copyfile( 
    lua_State *L
    )
{
  int status = 0;
  FILE *fp = NULL;
  char *X = NULL; size_t nX = 0;
  const char *const old_file = luaL_checkstring(L, 1);
  const char *const new_file = luaL_checkstring(L, 2);
  char *x_new_file = NULL; 
  if ( !isfile(old_file) ) { go_BYE(-1); }
  if ( isdir(new_file) ) {
    const char *dir = new_file; // we have been given directory not file 
    int len = strlen(dir) + strlen(old_file) + 8;
    x_new_file = malloc(len * sizeof(char));
    return_if_malloc_failed(x_new_file);
    sprintf(x_new_file, "%s/%s", dir, basename((char *)old_file));
  }
  else {
    x_new_file = strdup(new_file);
  }
  if ( strcmp(x_new_file, old_file) == 0 ) { go_BYE(-1); }
  status = rs_mmap(old_file, &X, &nX, 0); cBYE(status);
  fp = fopen(x_new_file, "w");
  // printf("%s, %s, %s \n", old_file, new_file, x_new_file);
  return_if_fopen_failed(fp, x_new_file, "w");
  fwrite(X, nX, 1, fp);
  fclose(fp);
  free(x_new_file);
  munmap(X, nX);
  lua_pushboolean(L, true);
  return 1; 
BYE:
  free_if_non_null(x_new_file);
  fclose_if_non_null(fp);
  if ( X != NULL ) { munmap(X, nX); }
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_write( 
    lua_State *L
    )
{
  FILE *fp = NULL;
  const char *const file_name = luaL_checkstring(L, 1);
  const char *const contents = luaL_checkstring(L, 2);
  fp = fopen(file_name, "w");
  if ( fp == NULL ) { WHEREAMI; goto BYE; }
  fprintf(fp, "%s",contents);
  fclose(fp);
  lua_pushboolean(L, true);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_quote_str( 
    lua_State *L
    )
{
  char *outstr = NULL;
  const char *const instr = luaL_checkstring(L, 1);
  int len = 2 + 2 * (strlen(instr) + 1);
  outstr = malloc(len);
  if ( outstr == NULL ) { WHEREAMI; goto BYE; }
  memset(outstr, '\0', len);
  int outidx = 0;
  outstr[outidx++] = '"';
  for ( const char *cptr = instr; *cptr != '\0'; cptr++ ) { 
    if ( ( *cptr == '"' ) ||  ( *cptr == '\\' ) ) { 
      outstr[outidx++] = '\\';
    }
    outstr[outidx++] = *cptr;
  }
  outstr[outidx++] = '"';
  lua_pushstring(L, outstr);
  free(outstr); 
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_read( 
    lua_State *L
    )
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *buf = NULL;
  const char *const file_name = luaL_checkstring(L, 1);
  status = rs_mmap(file_name, &X, &nX, 0); cBYE(status);
  buf = malloc(nX+1);
  return_if_malloc_failed(buf);
  memcpy(buf, X, nX);
  buf[nX] = '\0';
  lua_pushstring(L, buf);
  free(buf); // Wrote ../test/stress_test.lua to verify that this is okay
  munmap(X, nX);
  return 1; 
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_delete( 
    lua_State *L
    )
{
  const char *const file_name = luaL_checkstring(L, 1);
  if ( ( file_name == NULL ) || ( *file_name == '\0' ) )  {
    WHEREAMI; goto BYE;
  }
  bool exists = isfile(file_name);
  if ( !exists ) { lua_pushboolean(L, true); return 1; }
  //---------------
  int status = remove(file_name);
  if ( status != 0 ) { 
    fprintf(stderr, "Could not delete %s \n", file_name); 
    WHEREAMI; goto BYE; }
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static int l_memmon_currentdir( 
    lua_State *L
    )
{
  int bufsz = 1024;
  char buf[bufsz];
  memset(buf, '\0', bufsz);
  char *cptr = getcwd(buf, bufsz-1);
  if ( cptr != buf ) { WHEREAMI; goto BYE; }
  lua_pushstring(L, buf);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
#define DONT_CARE 0
#define ONLY_FILES 1
#define ONLY_DIRS  2
static int l_memmon_getfiles( 
    lua_State *L
    )
{
  int status = 0;
  struct stat st_buf;
  int mode = DONT_CARE;
  DIR *d = NULL;
  struct dirent *dir;
  char *full_file_name = NULL; 
  const char *mask = NULL;
  regex_t regex; int reti;
  const char *const dir_name = luaL_checkstring(L, 1);
  if ( ( dir_name == NULL ) || ( *dir_name == '\0' ) )  {
    WHEREAMI; goto BYE;
  }
  int len_dir_name = strlen(dir_name);
  int max_len_file_name = 256;
  int len_full_file_name = len_dir_name + max_len_file_name + 8;
  full_file_name = malloc(len_full_file_name * sizeof(char));
  return_if_malloc_failed(full_file_name);
  /* Use mask = ".*.c" to match .c files */
  if ( lua_gettop(L) >= 2 ) {
    mask  = luaL_checkstring(L, 2);
    if ( *mask != '\0' ) { 
      /* Compile regular expression */
      reti = regcomp(&regex, mask, 0);
      if ( reti != 0 ) { 
        fprintf(stderr, "Could not compile regex\n"); WHEREAMI; goto BYE;
      }
    }
  }
  if ( lua_gettop(L) >= 3 ) {
    const char *str_mode  = luaL_checkstring(L, 3);
    if ( strcasecmp(str_mode, "DONT_CARE") == 0 ) {
      mode = DONT_CARE;
    }
    else if ( strcasecmp(str_mode, "ONLY_FILES") == 0 ) {
      mode = ONLY_FILES;
    }
    else if ( strcasecmp(str_mode, "ONLY_DIRS") == 0 ) {
      mode = ONLY_DIRS;
    }
    else {
      WHEREAMI; goto BYE;
    }
  }

  //-------------
  d = opendir(dir_name);
  if ( d == NULL ) { 
    fprintf(stderr, "Could not opendir(%s)\n", dir_name); 
    WHEREAMI; goto BYE; 
  }
  // Now return table of strings
  lua_newtable(L);
  int dir_idx = 1;
  while ( (dir = readdir(d)) != NULL) {
    const char *file_name = dir->d_name;
    int len_file_name = strlen(file_name);
    if ( len_file_name > max_len_file_name ) { 
      max_len_file_name = len_file_name;
      len_full_file_name = len_dir_name + max_len_file_name + 8;
      free(full_file_name);
      full_file_name = malloc(len_full_file_name * sizeof(char));
      return_if_malloc_failed(full_file_name);
    }
    if ( mode != DONT_CARE ) {
      //-- decide whether file or directory
      sprintf(full_file_name, "%s/%s", dir_name, file_name);
      status = stat(full_file_name, &st_buf);
      if ( status != 0) { WHEREAMI; goto BYE; }

      if ( mode == ONLY_FILES ) { 
        if ( !S_ISREG (st_buf.st_mode)) {
          // printf("skipping %s because not a file\n", file_name);
          continue; 
        }
      }
      else if ( mode == ONLY_DIRS ) { 
        /* IMPORTANT: We do not return . or .. */
        if ( ( strcmp(file_name, "." ) == 0 ) || 
            ( strcmp(file_name, ".." ) == 0 )  ) {
          continue;
        }
        if ( !S_ISDIR (st_buf.st_mode)) {
          // printf("skipping %s because not a directory\n", file_name);
          continue; 
        }
      }
      else {
        WHEREAMI; goto BYE; 
      }
    }
    //---------
    bool include = false;
    if ( ( mask != NULL )  && ( *mask != '\0' ) ) { 
      reti = regexec(&regex, file_name, 0, NULL, 0);
    }
    if ( ( mask == NULL ) || ( *mask == '\0' ) ) {
      include = true;
    }
    else {
      if ( !reti ) { 
        include= true;
      }
    }
    if ( include ) {
      lua_pushnumber(L, dir_idx);
      lua_pushstring(L, file_name);
      lua_settable(L, -3);
      // printf("including %s \n", file_name);
      dir_idx++;
    }
    else {
      // printf("Excluding %s \n", file_name);
    }
  }
  closedir(d);
  if ( ( mask != NULL )  && ( *mask != '\0' ) ) { 
    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
  }
  free_if_non_null(full_file_name);
  return 1;
BYE:
  free_if_non_null(full_file_name);
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}
//----------------------------------------
static const struct luaL_Reg memmon_methods[] = {
    { NULL,  NULL         }
};
 
static const struct luaL_Reg memmon_functions[] = {
    { "malloc",    l_memmon_malloc },
    { "free",      l_memmon_free },
    { NULL,  NULL         }
};
 
/*
** Open test library
*/
int luaopen_libmemmon (lua_State *L) {
  /* Create the metatable and put it on the stack. */
  luaL_newmetatable(L, "memmon");
  /* Duplicate the metatable on the stack (We know have 2). */
  lua_pushvalue(L, -1);
  /* Pop the first metatable off the stack and assign it to __index
   * of the second one. We set the metatable for the table to itself.
   * This is equivalent to the following in lua:
   * metatable = {}
   * metatable.__index = metatable
   */
  lua_setfield(L, -2, "__index");

  /* Register the object.func functions into the table that is at the 
   * top of the stack. */

  /* Set the methods to the metatable that should be accessed via
   * object:func */
  luaL_register(L, NULL, memmon_methods);

  /* Register memmon in types table */
  int status = luaL_dostring(L, "return require 'MEMMON/lua/register_type'");
  if (status != 0 ) {
    printf("Running require failed:  %s\n", lua_tostring(L, -1));
    exit(1);
  } 
  luaL_getmetatable(L, "memmon");
  lua_pushstring(L, "memmon");
  status =  lua_pcall(L, 2, 0, 0);
  if (status != 0 ){
     printf("%d\n", status);
     printf("Registering type failed: %s\n", lua_tostring(L, -1));
     exit(1);
  }
  /* Register the object.func functions into the table that is at the
   op of the stack. */
  
  // Registering with Q
  status = luaL_dostring(L, "return require('Q/q_export').export");
  if (status != 0 ) {
    printf("Running Q registration require failed:  %s\n", lua_tostring(L, -1));
    exit(1);
  }
  lua_pushstring(L, "memmon");
  lua_createtable(L, 0, 0);
  luaL_register(L, NULL, memmon_functions);
  status = lua_pcall(L, 2, 1, 0);
  if (status != 0 ){
     printf("%d\n", status);
     printf("Registering with q_export failed: %s\n", lua_tostring(L, -1));
     exit(1);
  }
  
  return 1;
}
#ifdef OLD
  /* Register the object.func functions into the table that is at the
   * top of the stack. */
  lua_createtable(L, 0, 0);
  luaL_register(L, NULL, memmon_functions);

  return 1;
#endif
