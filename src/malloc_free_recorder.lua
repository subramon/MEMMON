-- table which records mallocs is T1
-- Key is the address returned by malloc
-- Value is a table like { __FILE__, __LINE__, __FUNC__, size  }
T1 = {}
--===============================================
record_malloc = function(addr, size, file, line, func)
  assert(type(addr) == "number")
  assert(type(size) == "number") assert(size > 0)
  assert(type(file) == "string")
  assert(type(line) == "number") assert(line > 0)
  assert(type(func) == "string")
  assert(not T1[addr]) -- cannot have 2 mallocs at same address
  T1[addr] = { file = file, line = line, func = func, size = size }
  return true
end

record_free = function(addr) 
  -- addr is the address that is being freed (as a 64 bit number)
  assert(type(addr) == "number")
  -- cannot free something that was not recorded in T1. hence, assert
  local t = assert(T1[addr])
  assert(type(t) == "table")
  local sz = assert(t.size)
  assert(type(sz) == "number")
  assert(sz > 0)
  T1[addr] = nil  --- free the spot 
  return sz
end
--===============================================
dump_mmon = function(file_name)
   assert(type(file_name) == 'string')
   -- need to do sudo luarocks install dkjson
   local dkj = require('dkjson') 
   local y = dkj.encode(T1) 
   local fp = assert(io.open(file_name, 'w')) 
   fp:write(y); 
   fp:close(); 
   return true
 end
--===============================================
