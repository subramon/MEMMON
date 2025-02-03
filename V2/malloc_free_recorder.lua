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
-- given a function name, return amount of mallocs/frees done
-- this requires another data structure T2
-- key is function name, value is a table 
-- { sz_malloc, num_malloc, sz_free, num_free }
T2 = {}
record_malloc_free_per_func = function(func, sz, op)
  assert(type(func) == 'string')
  assert(type(sz) == 'number')
  assert(sz > 0)
  assert(type(op) == 'string')
  assert((op == "malloc") or ( op == "free"))
  local t = T2[func]
  if ( t == nil ) then
    -- print("Adding entry for " .. func)
    -- HW: WHY IS THIS ASSERT WRONG? assert(op == "malloc")
    T2[func] = { sz_malloc = sz, num_malloc= 1, sz_free = 0, num_free = 0 } 
    return true
  end 
  assert(type(t) == "table") 
  if ( op == "malloc" ) then
    t.sz_malloc  = t.sz_malloc + sz
    t.num_malloc = t.num_malloc + 1
  elseif ( op == "free" ) then
    t.sz_free  = t.sz_free + sz
    t.num_free = t.num_free + 1
  else
    error("bad op " .. op)
  end
  T2[func] = t
 return true
end

get_malloc_free_per_func = function(func)
   assert(type(func) == 'string')
   local t = T2[func]
   if ( t == nil ) then return 0, 0, 0, 0 end 
   assert(type(t) == "table")
   return t.sz_malloc, t.num_malloc, t.sz_free, t.num_free
end

assert(type(get_malloc_free_per_func) == "function")
