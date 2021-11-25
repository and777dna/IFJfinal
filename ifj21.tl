require "ifj21"

function foo(x : integer, y : integer) : integer, integer
  local i : integer = x
  local j : integer = (y + 2) * 3
  i, j = j+1, i+1  -- vyhodnocuj zprava a p�i�azuj pozd�ji
  return i, j 
end

function main() 
  local a : integer = -125
  local b : integer = 2
  a, b = foo(a, b)  -- returns 13, 2
  while a <= b do
    write(a, "<", b, "\n")
    write(a, ">=", b, "\n")
    local a : integer = 666  -- Teal varuje, ze redefinice (prekryti)
    write(a, "\n")
  end
  write(a, "\n")  --[[ prints 13 ]]
end
main()
